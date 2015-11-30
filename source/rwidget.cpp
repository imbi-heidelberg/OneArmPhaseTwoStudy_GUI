#include "rwidget.h"
#include "ui_rwidget.h"
#include <QPaintEvent>


Rwidget::Rwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Rwidget), R(RInside::instance())
{
    ui->setupUi(this);
    // Get path for a temporary file which is used to draw the graphs produced in R.
    tmpFilePath = QString::fromStdString(Rcpp::as<std::string>(R.parseEval("imageTmpFile <- tempfile()")));
    image = new QImage();
    sacledImage = new QImage();
    studySet = false;
    this->displayPat = true;
    redrawTimer = new QTimer(this);
    sub1_stoppingrules_set = false;
    simon_stoppingrules_set = false;
    packageLoaded = false;
    connect(redrawTimer, SIGNAL(timeout()), this, SLOT(repaint()));

}

Rwidget::~Rwidget()
{
    delete ui;
    delete image;
    delete sacledImage;
    delete redrawTimer;
}

void Rwidget::resizeEvent(QResizeEvent *event)
{
    redrawTimer->start(); // Lillte hack to prevent timing issues between R and c++.
}

void Rwidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPen myPen1(Qt::blue,0.5,Qt::SolidLine,Qt::FlatCap, Qt::MiterJoin);

    if(!studySet) // Draw an red "X" if no study is set.
    {
        painter.setPen(myPen1);

        painter.drawLine(0,0,this->width()-1,0);
        painter.drawLine(0,0,0,this->height()-1);
        painter.drawLine(this->width()-1,this->height()-1,0,this->height()-1);
        painter.drawLine(this->width()-1,this->height()-1,this->width()-1,0);

        painter.drawLine(0,0,this->width()-1,this->height()-1);
        painter.drawLine(0,this->height()-1,this->width()-1,0);
    }
    else
    {
        QRectF rect(0,0, this->width(), this->height());
        *sacledImage = image->scaled(this->width(),this->height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        painter.drawImage(rect, *sacledImage);
    }
}

bool Rwidget::loadPackage()
{
    // Is the "OneArmPhaseTwo"-package installed?
    QString loadePackageCode;
    QTextStream ts(&loadePackageCode, QIODevice::ReadWrite);
    ts << "\"" << PACKAGE_NAME << "\" %in% rownames(installed.packages())";
    bool packageInstalled = false;
    packageInstalled = R.parseEval(loadePackageCode.toStdString());
    // If it is installed try to load it.
    if(packageInstalled)
    {
        loadePackageCode.clear();
        ts << "library(\"" << PACKAGE_NAME << "\")";
        R.parseEvalQ(loadePackageCode.toStdString());
        packageLoaded = true;
    }
    return packageInstalled && packageLoaded;
}

void Rwidget::setStudy(Study *study)
{
    sub1_stoppingrules_set = false;
    simon_stoppingrules_set = false;
    this->study = study;
    studySet = true;
    plotStudyState = study->getIsSimondesign();

    loadPackage();

    QObject::connect(study,SIGNAL(patientAdded(Study::Patient)),this,SLOT(newPatientAdded(Study::Patient)));
    QObject::connect(study, SIGNAL(patientRemoved()), this, SLOT(patientRemoved()));

    // Draw the study or the corresponding confidenceSet
    if(plotStudyState)
        drawStudyProgressToImage(); // Draw the study state (simon's tow stage design).
    else if(this->study->getFinished()) // The study is finished and a subset design is used.
        drawConfidenceSet(); // Draw the confidence set.
    else
        drawStudyProgressToImage_sub1(); // Draw the study state (subset design).

    image->load(tmpFilePath);
    this->repaint();
    QApplication::processEvents();

    // Little hack to ensure that the redraw process doesn't get lost.
    if(plotStudyState)
        drawStudyProgressToImage();
    else if(this->study->getFinished())
        drawConfidenceSet();
    else
        drawStudyProgressToImage_sub1();

    image->load(tmpFilePath);
    this->repaint();
}

void Rwidget::drawConfidenceSet()
{
    drawConfidenceSet(this->width(), this->height());
}

void Rwidget::drawConfidenceSet(int width, int height)
{
    tmpFilePath = QString::fromStdString(Rcpp::as<std::string>(R.parseEval("imageTmpFile")));
    Sub1_result *res = study->getSolutionStrategy_Sub1();

    QString openImageCode;
    QTextStream ts2(&openImageCode, QIODevice::ReadWrite);
    ts2 << "png(imageTmpFile, width=" << width << ",height =" << height << ");";
    std::string closeImageCode = "dev.off();";

    QString plotConfSet;
    QTextStream confSetTS(&plotConfSet, QIODevice::ReadWrite);

    confSetTS << "plot_confidence_set( t = " << study->getNumOfResponses() << ", u = " << study->getNumOfResponses_ep2() << ", r1 = " << res->getR1()
              << ", n1 = " << res->getN1() << ", n = " << res->getN() << ", pc0 = " << res->getPc0() << ", pt0 = " << res->getPt0()
              << ", alpha = " << study->getAlphaNominal() << ");";

    R.parseEvalQ(openImageCode.toStdString());
    R.parseEvalQ(plotConfSet.toStdString());
    R.parseEvalQ(closeImageCode);

    image->load(tmpFilePath);
}

void Rwidget::setDisplayPatients(bool displayPat)
{
    this->displayPat = displayPat;
}

void Rwidget::setSimonStoppingRulesSet(bool isSet)
{
    this->simon_stoppingrules_set = isSet;
    this->repaint();
}

void Rwidget::drawStudyProgressToImage()
{
    int minSize = 200;
    drawStudyProgressToImage(this->width() > minSize ? this->width() : minSize, this->height() > minSize ? this->height() : minSize);
}

void Rwidget::drawStudyProgressToImage(int width, int height)
{
    tmpFilePath = QString::fromStdString(Rcpp::as<std::string>(R.parseEval("imageTmpFile")));
    Result* res = this->study->getSolutionStrategy();

    // Are the stopping rules allready set in R? If false set them in R.
    if(!simon_stoppingrules_set)
    {
        QString stopRuleDataFrame;
        QTextStream txtStr(&stopRuleDataFrame, QIODevice::ReadWrite);
        R.parseEvalQ("sr = data.frame(Needed_responses_ep1 = numeric(), Needed_responses_ep2= numeric(), Enrolled_patients= numeric());");
        if(res->useCurtailment)
        {
            Result::Curtailment cur = (*res->curtailmentResults->find(res->cut)).second;
            std::vector<float*>::iterator it;

            float * tmp_sr;
            for(it = cur.stoppingRulesNSC->begin(); it != cur.stoppingRulesNSC->end(); it++)
            {
                tmp_sr = *it;
                txtStr << "sr = rbind( sr, data.frame( ";
                txtStr <<"Needed_responses_ep1 = " << tmp_sr[0];
                txtStr <<" ,Enrolled_patients = " << tmp_sr[1] << "));";
            }
            R.parseEvalQ(stopRuleDataFrame.toStdString());
        }
        else
        {
            //Stopping rule first stage
            txtStr << "sr = rbind( sr, data.frame( ";
            txtStr <<"Needed_responses_ep1 = " << res->r1;
            txtStr <<" ,Enrolled_patients = " << res->n1 << "));";

            //Stopping rule second stage
            txtStr << "sr = rbind( sr, data.frame( ";
            txtStr <<"Needed_responses_ep1 = " << res->r;
            txtStr <<" ,Enrolled_patients = " << res->n << "));";
            R.parseEvalQ(stopRuleDataFrame.toStdString());
        }
        simon_stoppingrules_set = true; // Remember that the stopping rules are now set in R.
    }

    std::vector<Study::Patient>::iterator it;

    QString enrolledPats;
    QTextStream enPatStr(&enrolledPats, QIODevice::ReadWrite);
    enPatStr << "enrolledPat = data.frame(ep1 = logical());";
    Study::Patient tmpPat;
    for(it = study->getEnrolledPatients()->begin(); it != study->getEnrolledPatients()->end(); it++)
    {
        tmpPat = *it;
        enPatStr << "enrolledPat = rbind(enrolledPat, data.frame(ep1 = " << (tmpPat.response ? "T":"F") << "));";
    }
    R.parseEvalQ(enrolledPats.toStdString());


    // Draw study state to the tmp-image.
    QString openImageCode;
    QTextStream ts2(&openImageCode, QIODevice::ReadWrite);
    ts2 << "png(imageTmpFile, width=" << width << ",height =" << height << ");";
    std::string closeImageCode = "dev.off();";

    QString drawCode;
    QTextStream drawCodeStr(&drawCode, QIODevice::ReadWrite);

    drawCodeStr << "plot_simon_study_state(sr = sr, enrolledPat =enrolledPat, r1 = " << res->r1 << ", n1 = "
                << res->n1 << ", r = " << res->r << ", n = " << res->n << ");";


    R.parseEvalQ(openImageCode.toStdString());
    R.parseEvalQ(drawCode.toStdString());
    R.parseEvalQ(closeImageCode);

    // Load the drawn tmp-image.
    image->load(tmpFilePath);

}

void Rwidget::drawStudyProgressToImage_sub1()
{
    int minSize = 200;
    drawStudyProgressToImage_sub1(this->width() > minSize ? this->width() : minSize, this->height() > minSize ? this->height() : minSize);
}

void Rwidget::drawStudyProgressToImage_sub1(int width, int height)
{
    tmpFilePath = QString::fromStdString(Rcpp::as<std::string>(R.parseEval("imageTmpFile")));
    Sub1_result* res = this->study->getSolutionStrategy_Sub1();

    // Are the stopping rules allready set in R? If false set them in R.
    if(!sub1_stoppingrules_set)
    {
        QString stopRuleDataFrame;
        QTextStream txtStr(&stopRuleDataFrame, QIODevice::ReadWrite);
        R.parseEvalQ("sr = data.frame(Needed_responses_ep1 = numeric(), Needed_responses_ep2= numeric(), Enrolled_patients= numeric());");
        if(res->getUseCurtailment())
        {
            Sub1_result::Curtailment_SubD1 cur = (*res->getCurtailmentResults()->find(res->getCut())).second;
            std::vector<Sub1_result::StoppingRule_SubD1>::iterator it;
            // Abbruchregeln werden in ein Dataframe zusammengefasst.

            Sub1_result::StoppingRule_SubD1 tmp_sr;
            for(it = cur.stoppingRulesNSC->begin(); it != cur.stoppingRulesNSC->end(); it++)
            {
                tmp_sr = *it;
                txtStr << "sr = rbind( sr, data.frame( ";
                txtStr <<"Needed_responses_ep1 = " << tmp_sr.t_int;
                txtStr <<" ,Needed_responses_ep2 = " << tmp_sr.u_int;
                txtStr <<" ,Enrolled_patients = " << tmp_sr.enrolled_int << "));";
            }
            R.parseEvalQ(stopRuleDataFrame.toStdString());
        }
        else
        {
            //Stopping rule first stage
            txtStr << "sr = rbind( sr, data.frame( ";
            txtStr <<"Needed_responses_ep1 = " << res->getR1();
            txtStr <<" ,Needed_responses_ep2 = " << res->getR1();
            txtStr <<" ,Enrolled_patients = " << res->getN1() << "));";

            //Stopping rule second stage
            txtStr << "sr = rbind( sr, data.frame( ";
            txtStr <<"Needed_responses_ep1 = " << res->getR();
            txtStr <<" ,Needed_responses_ep2 = " << res->getS();
            txtStr <<" ,Enrolled_patients = " << res->getN() << "));";
            R.parseEvalQ(stopRuleDataFrame.toStdString());
        }
        sub1_stoppingrules_set = true;


    }

    std::vector<Study::Patient>::iterator it;

    QString enrolledPats;
    QTextStream enPatStr(&enrolledPats, QIODevice::ReadWrite);
    enPatStr << "enrolledPat = data.frame(ep1 = logical(), ep2 = logical());";
    Study::Patient tmpPat;
    for(it = study->getEnrolledPatients()->begin(); it != study->getEnrolledPatients()->end(); it++)
    {
        tmpPat = *it;
        enPatStr << "enrolledPat = rbind(enrolledPat, data.frame(ep1 = " << (tmpPat.response ? "T":"F") << ", ep2 = " << (tmpPat.response_ep2 ? "T":"F") << "));";
    }
    R.parseEvalQ(enrolledPats.toStdString());


    // Draw study state to the tmp-image.
    QString openImageCode;
    QTextStream ts2(&openImageCode, QIODevice::ReadWrite);
    ts2 << "png(imageTmpFile, width=" << width << ",height =" << height << ");";
    std::string closeImageCode = "dev.off();";

    QString drawCode;
    QTextStream drawCodeStr(&drawCode, QIODevice::ReadWrite);

    drawCodeStr << "plot_sub1_study_state(sr = sr, enrolledPat =enrolledPat, r1 = " << res->getR1() << ", n1 = "
                << res->getN1() << ", r = " << res->getR() << ", s = " << res->getS() << ", n = " << res->getN() << ");";


    R.parseEvalQ(openImageCode.toStdString());
    R.parseEvalQ(drawCode.toStdString());
    R.parseEvalQ(closeImageCode);

    // Load the drawn tmp-image.
    image->load(tmpFilePath);
}

void Rwidget::repaint()
{
    if(redrawTimer->isActive())
        redrawTimer->stop();
    if(this->plotStudyState)
        drawStudyProgressToImage();
    else if(this->study->getFinished())
        drawConfidenceSet();
    else
        drawStudyProgressToImage_sub1();

    QWidget::repaint();
}


void Rwidget::newPatientAdded(Study::Patient p)
{
    this->repaint();
}

void Rwidget::patientRemoved()
{
    this->repaint();
}




