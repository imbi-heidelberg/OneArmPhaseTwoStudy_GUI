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

#include "newstudyoverview.h"
#include "ui_newstudyoverview.h"

NewStudyOverviewPage::NewStudyOverviewPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewStudyOverviewPage)
{
    ui->setupUi(this);
}

NewStudyOverviewPage::~NewStudyOverviewPage()
{
    delete ui;
}

void NewStudyOverviewPage::showStudyOverview(Study *study)
{

    ui->rw_overview->setDisplayPatients(false);
    ui->rw_overview->setStudy(study);
    ui->rw_overview->show();

    // Display gernal study-informations.
    ui->l_StudyName->setText("<b>Study name</b>: " + study->getStudyName());
    ui->l_Investigator->setText("<b>Investigator</b>: " + study->getInvestigator());
    ui->l_Biometrician->setText("<b>Biometrician</b>: " + study->getBiometrician());
    ui->l_StudyType->setText("<b>Study type</b>: Simondesign");  //noch hardcodiert

    //Display informations about the selected design.
    if(study->getIsSimondesign())
    {
        Result* solStrat = study->getSolutionStrategy();
        ui->l_SolutionStategy->setText(*(solStrat->name));

        QString s;
        QTextStream ts(&s, QIODevice::ReadWrite);
        ts.setRealNumberPrecision(2);

        ts << "<b>First stage</b> : " << solStrat->r1 << "/" << solStrat->n1;
        ui->l_r1_n1->setText(s);
        s.clear();
        ts << "<b>Second stage</b> : " << solStrat->r << "/" << solStrat->n;
        ui->l_r_n->setText(s);
        s.clear();
        ts << "<b>EN(p0)</b> = " << solStrat->enP0 ;
        ui->l_enp0->setText(s);
        s.clear();
        ts << "<b>PET(p0)</b> = " << solStrat->petP0 ;
        ui->l_petP0->setText(s);
        s.clear();
        ts << "<b>Alpha</b> = " << solStrat->alpha ;
        ui->l_alpha->setText(s);
        s.clear();
        ts << "<b>Beta</b> = " << solStrat->beta ;
        ui->l_beta->setText(s);

        // Display informations about stochastic curtailment if used.
        if(solStrat->useCurtailment)
        {
            ui->gB_Curtailment->show();
            s.clear();
            ts << "<b>Cut</b> = " << solStrat->cut << "%";
            ui->l_cut->setText(s);

            Result::Curtailment curt = (*solStrat->curtailmentResults->find(solStrat->cut)).second;

            s.clear();
            ts << "<b>Alpha_SC</b >= " << curt.type1_errorRate;
            ui->l_alphaSC->setText(s);
            s.clear();
            ts << "<b>Beta_SC</b> = " << curt.type2_errorRate;
            ui->l_betaSC->setText(s);
            s.clear();
            ts << "<b>EN(p0)_SC</b> = " << curt.en_sc;
            ui->l_enp0SC->setText(s);
            s.clear();
            ts << "<b>PET(p0)_SC</b> =" << curt.pet_sc;
            ui->l_petP0SC->setText(s);

        }
        else
            ui->gB_Curtailment->hide();
        // Display informations about the "flexible extension" if used.
        if(solStrat->useFlexible)
        {
            QString value;
            QTableWidgetItem *item;
            std::map<int, double>::iterator it;
            std::map<int, double> *flexValues;
            switch(solStrat->flexSol)
            {
            case Result::Proportional:
                flexValues = solStrat->flexibleAdaptionProp;
                ui->l_distributeMethod->setText("<b>Distribution method:<b> Proportional.");
                break;
            case Result::Equal:
                flexValues = solStrat->flexibleAdaptionEqua;
                ui->l_distributeMethod->setText("<b>Distribution method:<b> Equal.");
                break;
            case Result::ToOne:
                flexValues = solStrat->flexibleAdaptionToOne;
                ui->l_distributeMethod->setText("<b>Distribution method:<b> Smallest.");
                break;
            case Result::None:
                flexValues = solStrat->flexibleAdaptionNone;
                ui->l_distributeMethod->setText("<b>Distribution method:<b> None.");
                break;
            default:
                break;
            }

            it = flexValues->begin();

            int k = 0;
            double ce;
            while( it != flexValues->end())
            {
                //increase row count to display data
                ui->t_flexible_Overview->setRowCount(k +1);
                //add data to the three tableWidgets
                value = QString::number(k);
                item = new QTableWidgetItem(value);
                ui->t_flexible_Overview->setItem(k, 0, item);

                ce = it->second;
                value.sprintf("%.4f",ce);
                item = new QTableWidgetItem(value);
                ui->t_flexible_Overview->setItem(k, 1, item);

                k++;
                it++;
            }
            ui->gB_flexibleOverview->show();
        }
        else
            ui->gB_flexibleOverview->hide();
    }
    else
    {
        ui->gB_flexibleOverview->hide();

        Sub1_result* solStrat = study->getSolutionStrategy_Sub1();
        ui->l_SolutionStategy->setText(*(solStrat->getName()));

        QString s;
        QTextStream ts(&s, QIODevice::ReadWrite);
        ts.setRealNumberPrecision(2);

        ts << "<b>First stage</b> : " << solStrat->getR1() << "/" << solStrat->getN1();
        ui->l_r1_n1->setText(s);
        s.clear();
        ts << "<b>Second stage</b> : " << solStrat->getR() << "," << solStrat->getS() << "/" << solStrat->getN();
        ui->l_r_n->setText(s);
        s.clear();
        ts << "<b>EN(p0)</b> = " << solStrat->getEnP0() ;
        ui->l_enp0->setText(s);
        s.clear();
        ts << "<b>PET(p0)</b> = " << solStrat->getPetP0() ;
        ui->l_petP0->setText(s);
        s.clear();
        ts << "<b>Alpha</b> = " << solStrat->getAlpha() ;
        ui->l_alpha->setText(s);
        s.clear();
        ts << "<b>Beta</b> = " << solStrat->getBeta() ;
        ui->l_beta->setText(s);

        // Display informations about stochastic curtailment if used.
        if(solStrat->getUseCurtailment())
        {
            ui->gB_Curtailment->show();
            s.clear();
            ts << "<b>Cut</b> = " << solStrat->getCut() << "%";
            ui->l_cut->setText(s);

            Sub1_result::Curtailment_SubD1 curt = (*solStrat->getCurtailmentResults()->find(solStrat->getCut())).second;

            s.clear();
            ts << "<b>Alpha_SC</b >= " << curt.type1_errorRate;
            ui->l_alphaSC->setText(s);
            s.clear();
            ts << "<b>Beta_SC</b> = " << curt.type2_errorRate;
            ui->l_betaSC->setText(s);
            s.clear();
            ts << "<b>EN(p0)_SC</b> = " << curt.en_sc;
            ui->l_enp0SC->setText(s);
            s.clear();
            ts << "<b>PET(p0)_SC</b> =" << curt.pet_sc;
            ui->l_petP0SC->setText(s);

        }
        else
            ui->gB_Curtailment->hide();
    }


}
