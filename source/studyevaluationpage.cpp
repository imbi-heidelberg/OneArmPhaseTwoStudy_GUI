// This file is part of OneArmPhaseTwoStudy.
//
// OneArmPhaseTwoStudy is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// OneArmPhaseTwoStudy is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details (http://www.gnu.org/licenses/).

#include "studyevaluationpage.h"
#include "ui_studyevaluationpage.h"

StudyEvaluationPage::StudyEvaluationPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudyEvaluationPage)
{
    ui->setupUi(this);
    this->simon = NULL;
    this->sub1 = NULL;
}

StudyEvaluationPage::~StudyEvaluationPage()
{
    delete ui;
    if(simon != NULL)
        delete simon;
    if(sub1 != NULL)
        delete sub1;
}

void StudyEvaluationPage::setStudy(Study *study)
{
    this->currentStudy = study;
    this->updateView(study);
    ui->w_graph->setStudy(study);

}

void StudyEvaluationPage::updateView(Study *study)
{
    QString s;
    QTextStream ts(&s, QIODevice::ReadWrite);
    ts.setRealNumberPrecision(2);

    if(study->getIsSimondesign())
    {
        ui->l_pt0->hide();
        ui->l_pt1->hide();
        ui->l_ml_ep2->hide();
        ui->l_umvue_ep2->hide();
        ui->l_responses_ep2->hide();

        ui->gB_GraficalOverview->show();
        ui->w_graph->show();

        Result *res = study->getSolutionStrategy();

        ts << "<b>r1/n1</b>: " << res->r1 << "/"<< res->n1;
        ui->l_r1n1->setText(s);
        s.clear();
        ts << "<b>r/n</b>: " << res->r << "/" << res->n;
        ui->l_rn->setText(s);
        s.clear();
        ts << "<b>p0</b>: " << res->p0;
        ui->l_p0->setText(s);
        s.clear();
        ts << "<b>p1</b>: " << res->p1;
        ui->l_p1->setText(s);
        s.clear();
        ts << "<b>EN(p0)</b>: " << res->enP0;
        ui->l_enp0->setText(s);
        s.clear();
        ts << "<b>PET(p0)</b>: " << res->petP0;
        ui->l_petp0->setText(s);
        s.clear();
        ts << "<b>Alpha</b>: " << res->alpha;
        ui->l_alpha->setText(s);
        s.clear();
        ts << "<b>Beta</b>: " << res->beta;
        ui->l_beta->setText(s);
        s.clear();
        if(res->admissible)
        {
            ts << "<b>Admissible</b>: True";
            ui->l_admissible->setText(s);
            s.clear();
            ts << "<b>Admissible-range</b>: " << res->admissibleStart << " &lt;-&gt; " << res->admissibleStop;
            ui->l_admissRange->setText(s);
            s.clear();
            ts << "<b>Solution typ</b>: " << *(res->name);
            ui->l_desgintype->setText(s);
            s.clear();
        }
        else
        {
            ui->l_admissible->hide();
            ui->l_admissRange->hide();
            ui->l_desgintype->hide();
        }

        ts << "<b>Enrolled patients</b>: " << study->getEnrolledPatients()->size();
        ui->l_enrolledPat->setText(s);
        s.clear();
        ts << "<b>Responses</b>: " << study->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();

        if(simon == NULL)
        {
            simon = new R_simondesign();
            simon->loadPackage();
        }
        // Begin calculation of the UMVUE and the corresponding CI
        // as well as the maximum likelihood estimator (ml) and p-value.
        double umvue = 0;
        double ml = 0;
        double pValue = 0;
        double *ci = NULL;
        int k, r1, n1, n;
        k = currentStudy->getNumOfResponses();
        r1 = res->r1;
        n1 = res->n1;
        n = res->n;

        ts.setRealNumberPrecision(4);
        ml = (double)k / (double)(currentStudy->getEnrolledPatients()->size());
        ts << "<b>ML</b>: " << ml;
        ui->l_ml->setText(s);
        s.clear();
        umvue = simon->get_UMVUE(k, r1, n1, n);
        ts << "<b>UMVUE</b>: " << umvue;
        ui->l_umvue->setText(s);
        s.clear();
        if(currentStudy->getSolutionStrategy()->useFlexible)
        {
            ui->l_ce_flex->show();
            double ce = currentStudy->getCE_flex();
            ts << "<b>Conditional error</b>: " << ce;
            ui->l_ce_flex->setText(s);
            s.clear();
            pValue = simon->getPvalueSingelStage((k - currentStudy->getNumOfResponses_stageOne()), res->p0,n - n1);
            ts << "<b>P-value second stage</b>: " << pValue;
            ui->l_pValue->setText(s);
            s.clear();


        }
        else
        {
            pValue = simon->get_pValue(k, r1, n1, n, res->p0);
            ts << "<b>P-value</b>: " << pValue;
            ui->l_pValue->setText(s);
            s.clear();
            ui->l_ce_flex->hide();
        }

        ci = simon->get_CI(k, r1, n1, n, 0.05, 4);
        ts << "<b>CI</b>: [" << ci[0] << ", " << ci[1] << "]";
        ui->l_ci->setText(s);
        s.clear();
        delete[] ci;
    }
    else
    {
        Sub1_result *res = study->getSolutionStrategy_Sub1();
        ui->l_ce_flex->hide();

        ts << "<b>r1/n1</b>: " << res->getR1() << "/"<< res->getN1();
        ui->l_r1n1->setText(s);
        s.clear();
        ts << "<b>r,s/n</b>: " << res->getR() << "," << res->getS() << "/" << res->getN();
        ui->l_rn->setText(s);
        s.clear();
        ts << "<b>pc0</b>: " << res->getPc0();
        ui->l_p0->setText(s);
        s.clear();
        ts << "<b>pt0</b>: " << res->getPt0();
        ui->l_pt0->setText(s);
        s.clear();
        ts << "<b>pc1</b>: " << res->getPc1();
        ui->l_p1->setText(s);
        s.clear();
        ts << "<b>pt1</b>: " << res->getPt1();
        ui->l_pt1->setText(s);
        s.clear();
        ts << "<b>EN(p0)</b>: " << res->getEnP0();
        ui->l_enp0->setText(s);
        s.clear();
        ts << "<b>PET(p0)</b>: " << res->getPetP0();
        ui->l_petp0->setText(s);
        s.clear();
        ts << "<b>Alpha</b>: " << res->getAlpha();
        ui->l_alpha->setText(s);
        s.clear();
        ts << "<b>Beta</b>: " << res->getBeta();
        ui->l_beta->setText(s);
        s.clear();
        if(res->getAdmissible())
        {
            ts << "<b>Admissible</b>: True";
            ui->l_admissible->setText(s);
            s.clear();
            ts << "<b>Admissible-range</b>: " << res->getAdmissibleStart() << " &lt;-&gt; " << res->getAdmissibleStop();
            ui->l_admissRange->setText(s);
            s.clear();
            ts << "<b>Solution typ</b>: " << *(res->getName());
            ui->l_desgintype->setText(s);
            s.clear();
        }
        else
        {
            ui->l_admissible->hide();
            ui->l_admissRange->hide();
            ui->l_desgintype->hide();
        }

        ts << "<b>Enrolled patients</b>: " << study->getEnrolledPatients()->size();
        ui->l_enrolledPat->setText(s);
        s.clear();
        ts << "<b>Responses Endpoint One</b>: " << study->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();
        ts << "<b>Responses Endpoint Two</b>: " << study->getNumOfResponses_ep2();
        ui->l_responses_ep2->setText(s);
        s.clear();

        if(sub1 == NULL)
        {
            sub1 = new R_Sub1Design();
            sub1->loadPackage();
        }
        // Begin calculation of the UMVUE (for both endpoints) and the corresponding CI
        // as well as the maximum likelihood estimator (ml) and p-value.
        double umvue = 0;
        double umvue_ep2 = 0;
        double ml = 0;
        double ml_ep2 = 0;
        double pValue = 0;
        int t, u, r1, n1, n;
        t = currentStudy->getNumOfResponses();
        u = currentStudy->getNumOfResponses_ep2();
        r1 = res->getR1();
        n1 = res->getN1();
        n = res->getN();

        ts.setRealNumberPrecision(4);
        ml = (double)t / (double)(currentStudy->getEnrolledPatients()->size());
        ts << "<b>ML Endpoint One</b>: " << ml;
        ui->l_ml->setText(s);
        s.clear();
        ml_ep2 = (double)u / (double)(currentStudy->getEnrolledPatients()->size());
        ts << "<b>ML Endpoint Two</b>: " << ml_ep2;
        ui->l_ml_ep2->setText(s);
        s.clear();
        umvue = sub1->get_UMVUE_ep1(t, r1, n1, n);
        ts << "<b>UMVUE Endpoint One</b>: " << umvue;
        ui->l_umvue->setText(s);
        s.clear();
        umvue_ep2 = sub1->get_UMVUE_ep2(t, u, r1, n1, n);
        ts << "<b>UMVUE Endpoint Two</b>: " << umvue_ep2;
        ui->l_umvue_ep2->setText(s);
        s.clear();

        pValue = sub1->get_pValue(t, u, r1, n1, n, res->getPc0(), res->getPt0());
        ts << "<b>P-value</b>: " << pValue;
        ui->l_pValue->setText(s);
        s.clear();
        ui->l_ci->hide();
    }



    this->repaint();
}
