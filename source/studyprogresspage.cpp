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

#include "studyprogresspage.h"
#include "ui_studyprogresspage.h"


StudyProgressPage::StudyProgressPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudyProgressPage)
{
    ui->setupUi(this);
    ui->dE_Eventdate->setDate(QDate::currentDate());
    ui->dE_Recroutdate->setDate(QDate::currentDate());
    ui->pB_AddPatient->setEnabled(false);
    ui->t_EnrolledPatients->setEditTriggers(QAbstractItemView::NoEditTriggers);

    simon = NULL;
    sub1 = NULL;

    ui->gB_flex->hide();
    flex_cp_changed = false;
    flex_n_changed = false;
    tmp_r_flex = 0;

    // Set tooltips for the differnt GUI elements
    ui->l_alpha->setToolTip("Probability for a type one error.");
    ui->l_beta->setToolTip("Probability for a type two error.");
    ui->l_cp->setToolTip("Conditional power for the given state of the study.");
    ui->l_cut->setToolTip("Chosen cut for the (non) statistical curtailment");
    ui->l_enp0->setToolTip("Expected number of patients involved in the study given the zero hypothesis holds true.");
    ui->l_enrolledPats->setToolTip("Enrolled patients so far.");
    ui->l_n->setToolTip("Over all needed patients.");
    ui->l_n1->setToolTip("Needed patients for the first stage of the study.");
    ui->l_petp0->setToolTip("Propability for an early termination of the study given that the zero hypothesis is true.");
    ui->l_r->setToolTip("Responses in this study must be higher than this value to reject the zero hypothesis.");
    ui->l_r1->setToolTip("Number of responses must be higer than this value to proceed to the second stage of the study.");
    ui->l_responses->setToolTip("Observed responses so far.");

    // Hide currently obsolete GUI elements
    ui->label_5->hide();
    ui->label_6->hide();
    ui->dE_Eventdate->hide();
    ui->dE_Recroutdate->hide();
    ui->cB_Response->hide();
    ui->l_n->hide();
    ui->l_n1->hide();
}

StudyProgressPage::~StudyProgressPage()
{
    delete ui;
    if(simon != NULL)
        delete simon;
}

void StudyProgressPage::setStudy(Study *study)
{
    this->currentStudy = study;
    if(study->getIsSimondesign())
    {
        // If the current study uses a "simon's two stage design" the column for endpoint two is not needed.
        ui->w_Rplot->setStudy(study);
        if(ui->t_EnrolledPatients->columnCount() >= 3)
            ui->t_EnrolledPatients->removeColumn(2);
        ui->t_EnrolledPatients->horizontalHeaderItem(1)->setText("Response");
        ui->gB_response_detail->hide();
    }
    else
    {
        // If the current study uses a "subset design" the column for endpoint two is needed.
        ui->w_Rplot->setStudy(study);
        ui->t_EnrolledPatients->insertColumn(2);
        ui->t_EnrolledPatients->setHorizontalHeaderItem(2, new QTableWidgetItem("Response endpoint two"));
        ui->t_EnrolledPatients->horizontalHeaderItem(1)->setText("Response endpoint one");
        ui->gB_response_detail->show();
    }

    ui->l_Studyname->setText("<b>Studyname</b>: "+ currentStudy->getStudyName());
    ui->l_Investigator->setText("<b>Investigator</b>: " + currentStudy->getInvestigator());
    ui->l_Biometrician->setText("<b>Biometrician</b>: " + currentStudy->getBiometrician());

    showStudyDetails(study);


    // Allready enrolled patients are added to the corresponding table.
    ui->t_EnrolledPatients->clearContents();
    ui->t_EnrolledPatients->setRowCount(0);
    std::vector<Study::Patient>::iterator it;

    for(it = currentStudy->getEnrolledPatients()->begin(); it != currentStudy->getEnrolledPatients()->end(); it++)
    {
        this->addPatientToTable((*it));
    }    


}

void StudyProgressPage::showStudyDetails(Study *study)
{
    QString s;
    QTextStream ts(&s, QIODevice::ReadWrite);
    ts.setRealNumberPrecision(2);

    if(study->getIsSimondesign())
    {
        // Display all available informations about the study (including the used "simons's two stage design").
        Result *res = study->getSolutionStrategy();
        ui->l_pt1->hide();
        ui->l_pt0->hide();
        ui->l_responses_ep2->hide();

        ts << "<b>r/n</b>: " << res->r << "/" << res->n;
        ui->l_r->setText(s);
        s.clear();
        ts << "<b>r1/n1</b>: " << res->r1 << "/"<< res->n1;
        ui->l_r1->setText(s);
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
            ui->l_admiss->setText(s);
            s.clear();
            ts << "<b>Admissible-range</b>: " << res->admissibleStart << " &lt;-&gt; " << res->admissibleStop;
            ui->l_admissrange->setText(s);
            s.clear();
            ts << "<b>Solution typ</b>: " << *(res->name);
            ui->l_solname->setText(s);
            s.clear();
        }
        else
        {
            // Hide gui elements which are unnecessary for the current study ( /are not used by the current study).
            ui->l_admiss->hide();
            ui->l_admissrange->hide();
            ui->l_solname->hide();
        }
        ts << "<b>Enrolled patients</b>: " << study->getEnrolledPatients()->size();
        ui->l_enrolledPats->setText(s);
        s.clear();
        ts << "<b>Responses</b>: " << study->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();
        if(simon == NULL)
        {
            simon = new R_simondesign();
            simon->loadPackage();
        }
        ts << "<b>Conditional power</b>: " << simon->getConditionalPower(study->getNumOfResponses(), study->getEnrolledPatients()->size(), res->r1, res->n1, res->r, res->n, res->p1);
        ui->l_cp->setText(s);
        s.clear();
        if(res->useCurtailment)
        {
            ts << "<b>Cut</b>: " << res->cut <<"&#37;";
            ui->l_cut->setText(s);
            s.clear();
        }
        else
        {
            // Hide gui elements which are unnecessary for the current study ( /are not used by the current study).
            ui->gB_Curtailment->hide();
            ui->l_cut->hide();
        }
    }
    else
    {
        // Display all available informations about the study (including the used "subset design").
        Sub1_result *res = study->getSolutionStrategy_Sub1();

        ui->l_pt1->show();
        ui->l_pt0->show();
        ui->l_responses_ep2->show();
        ts << "<b>r,s/n</b>: " << res->getR() << "," << res->getS() << "/" << res->getN();
        ui->l_r->setText(s);
        s.clear();
        ts << "<b>r1/n1</b>: " << res->getR1() << "/"<< res->getN1();
        ui->l_r1->setText(s);
        s.clear();
        ts << "<b>pc0</b>: " << res->getPc0();
        ui->l_p0->setText(s);
        s.clear();
        ts << "<b>pt0</b>: " << res->getPt0();
        ui->l_pt0->setText(s);
        s.clear();
        ts << "<b>p1</b>: " << res->getPc1();
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
            ui->l_admiss->setText(s);
            s.clear();
            ts << "<b>Admissible-range</b>: " << res->getAdmissibleStart() << " &lt;-&gt; " << res->getAdmissibleStop();
            ui->l_admissrange->setText(s);
            s.clear();
            ts << "<b>Solution typ</b>: " << *(res->getName());
            ui->l_solname->setText(s);
            s.clear();
        }
        else
        {
            // Hide gui elements which are unnecessary for the current study ( /are not used by the current study).
            ui->l_admiss->hide();
            ui->l_admissrange->hide();
            ui->l_solname->hide();
        }
        ts << "<b>Enrolled patients</b>: " << study->getEnrolledPatients()->size();
        ui->l_enrolledPats->setText(s);
        s.clear();
        ts << "<b>Responses endpoint one</b>: " << study->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();
        ts << "<b>Responses endpoint two</b>: " << study->getNumOfResponses_ep2();
        ui->l_responses_ep2->setText(s);
        s.clear();
        if(sub1 == NULL)
        {
            sub1 = new R_Sub1Design();
            sub1->loadPackage();
        }
        ts << "<b>Conditional power</b>: " << sub1->get_conditionalPower(study->getNumOfResponses(), study->getNumOfResponses_ep2() ,study->getEnrolledPatients()->size(), res->getR1(), res->getN1(), res->getR(), res->getS(), res->getN(), res->getPc1(), res->getPt1());
        ui->l_cp->setText(s);
        s.clear();
        if(res->getUseCurtailment())
        {
            ts << "<b>Cut</b>: " << res->getCut() <<"&#37;";
            ui->l_cut->setText(s);
            s.clear();
        }
        else
        {
            // Hide gui elements which are unnecessary for the current study ( /are not used by the current study).
            ui->gB_Curtailment->hide();
            ui->l_cut->hide();
        }
    }


}

void StudyProgressPage::on_pB_AddPatient_clicked()
{
    // Adding a new patient cn result in different consequences for the current study.
    //      1. The study continues without any changes.
    //      2. A stopping rule gets violated and the study has to be stopped for futility.
    //      3. "n1" patients are enrolled and the interim analysis has to be performed.
    //      4. If currently the interim analysis is performed adding a new patients stops the interim analysis.

    // Only add new patients if the study is not stopped.
    if(!(this->currentStudy->getStopped()))
    {
        Study::Patient p;
        p.patID = ui->lE_patID->text();

        if(currentStudy->getIsSimondesign())
        {
            p.response = ui->rb_response->isChecked() ? true : false;
        }
        else
        {
            p.response = ui->cB_ep1->isChecked() ? true : false;
            p.response_ep2 = ui->cB_ep2->isChecked() ? true : false;

            ui->cB_ep2->setChecked(false);
            ui->cB_ep1->setChecked(false);
        }

        ui->rb_response->setCheckable(false);
        ui->rb_noResponse->setCheckable(false);
        ui->rb_response->setCheckable(true);
        ui->rb_noResponse->setCheckable(true);

        bool prossed = true;

        // If currently the interim analysis is performed, the user gets informed (through a message box)
        // about the fact that adding a new patients will stop the interim analysis and the second
        // enrollment phase of the study starts.
        if(currentStudy->getIsSimondesign())
        {
            if((currentStudy->getSolutionStrategy()->n1 == (int)(currentStudy->getEnrolledPatients()->size()))
                    && (currentStudy->getSolutionStrategy()->useFlexible))
            {
                // If the flexible extension is used it will be impossible to alter the sample size of
                // the second phase of the study if the interim analysis is stopped.
                if(currentStudy->getSolutionStrategy()->useFlexible)
                {
                    QMessageBox msgBox;
                    msgBox.setInformativeText("Add patient?");
                    msgBox.setText("If you add new patients the interim analysis is over \nand you will not be able to make any changes regarding to the flexible extendition. \nAdd the patient?");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setIcon(QMessageBox::Information);
                    int ret = msgBox.exec();
                    if( ret != QMessageBox::Yes)
                        prossed = false;
                    else
                    {
                        ui->gB_flex->hide();
                    }
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setInformativeText("Add patient?");
                    msgBox.setText("If you add new patients the interim analysis is over. \nAdd the patient?");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setIcon(QMessageBox::Information);
                    int ret = msgBox.exec();
                    if( ret != QMessageBox::Yes)
                        prossed = false;
                }
            }
        }
        else
        {
            if(currentStudy->getSolutionStrategy_Sub1()->getN1() == (int)(currentStudy->getEnrolledPatients()->size()))
            {
                QMessageBox msgBox;
                msgBox.setInformativeText("Add patient?");
                msgBox.setText("If you add new patients the interim analysis is over. \nAdd the patient?");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setIcon(QMessageBox::Information);
                int ret = msgBox.exec();
                if( ret != QMessageBox::Yes)
                    prossed = false;
            }
        }


        if(prossed)
        {
            if(currentStudy->addPatient(p))
            {
                this->addPatientToTable(p);
                this->updateDetails();
                ui->pB_removePat->setEnabled(true);
                ui->lE_patID->setText("");

                // Check if the study has to be stopped for futility (if stochastic curtailment is used).
                if(currentStudy->getIsSimondesign())
                {
                    if(currentStudy->stopForFutility() && currentStudy->getSolutionStrategy()->useCurtailment)
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Study can be stopped for futility regarding to the stopping rules.");
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.exec();
                        this->currentStudy->setStopped(true);
                    }
                }
                else
                {
                    if(currentStudy->stopForFutility() && currentStudy->getSolutionStrategy_Sub1()->getUseCurtailment())
                    {
                        QMessageBox msgBox;
                        msgBox.setText("Study can be stopped for futility regarding to the stopping rules.");
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.exec();
                        this->currentStudy->setStopped(true);
                    }
                }

                if(currentStudy->getIsSimondesign())
                {
                    if((size_t)(currentStudy->getSolutionStrategy()->n1) == currentStudy->getEnrolledPatients()->size())
                    {
                        // Check if "n1" patients are enrolled. (If there are in fact "n1" patients are enrolled the interim analysis is performed.)
                        if(currentStudy->getNumOfResponses() > currentStudy->getSolutionStrategy()->r1)
                        {
                            // If the felxible extension is used the sample size of the secondphase of the study ("n2") can be altered.
                            if(currentStudy->getSolutionStrategy()->useFlexible)
                            {
                                QMessageBox msgBox;
                                msgBox.setText("You have reached the interim analysis. \nYou are using the flexible extension so you may now change the number of patients to be recruited.");
                                msgBox.setIcon(QMessageBox::Information);
                                msgBox.exec();

                                Result *res = currentStudy->getSolutionStrategy();
                                ui->sB_flex_n->setValue(res->n);
                                ui->dsb_flex_cp->setValue(simon->getCP_flex(res->n - res->n1, res, currentStudy->getNumOfResponses(), currentStudy->getAlphaNominal()));
                                ui->tabWidget->setCurrentIndex(0);
                                ui->gB_flex->show();
                                ui->gB_flex->setFocus();
                            }
                            else
                            {
                                QMessageBox msgBox;
                                msgBox.setText("You have reached the interim analysis. ");
                                msgBox.setIcon(QMessageBox::Information);
                                msgBox.exec();

                                ui->tabWidget->setCurrentIndex(0);
                            }
                        }
                        else // There are not enough responses among the "n1" enrolled patients to procceed with the second phase of the study.
                        {
                            QMessageBox msgBox;
                            msgBox.setText("You have reached the interim analysis. \nAccording to the chosen study design, there are not enough responses observed to procceed to the next stage of the study. \nThe study is to be stopped with acceptance of the null hypothesis.");
                            msgBox.setIcon(QMessageBox::Information);
                            msgBox.exec();
                            this->currentStudy->setStopped(true);
                        }

                    }
                    else if((size_t)(currentStudy->getSolutionStrategy()->n) <= currentStudy->getEnrolledPatients()->size())
                    {
                        // There are "n" patients enrolled in total. Therefore the study is finished and the "studyFinished" is emited.
                        this->currentStudy->setFinished(true);
                        emit studyFinished(true);
                    }
                }
                else
                {
                    if((size_t)(currentStudy->getSolutionStrategy_Sub1()->getN1()) == currentStudy->getEnrolledPatients()->size())
                    {
                        // Check if "n1" patients are enrolled. (If there are in fact "n1" patients are enrolled the interim analysis is performed.)
                        if(currentStudy->getNumOfResponses() > currentStudy->getSolutionStrategy_Sub1()->getR1())
                        {
                            QMessageBox msgBox;
                            msgBox.setText("You have reached the interim analysis. ");
                            msgBox.setIcon(QMessageBox::Information);
                            msgBox.exec();

                            ui->tabWidget->setCurrentIndex(0);

                        }
                        else // There are not enough responses among the "n1" enrolled patients to procceed with the second phase of the study.
                        {
                            QMessageBox msgBox;
                            msgBox.setText("You have reached the interim analysis. \nAccording to the chosen study design, there are not enough responses observed to procceed to the next stage of the study. \nThe study is to be stopped with acceptance of the null hypothesis.");
                            msgBox.setIcon(QMessageBox::Information);
                            msgBox.exec();
                            this->currentStudy->setStopped(true);
                        }

                    }
                    else if((size_t)(currentStudy->getSolutionStrategy_Sub1()->getN()) <= currentStudy->getEnrolledPatients()->size())
                    {
                        // There are "n" patients enrolled in total. Therefore the study is finished and the "studyFinished" is emited.
                        this->currentStudy->setFinished(true);
                        emit studyFinished(true);
                    }
                }

            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText("Patient already enrolled.");
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
            }
        }
    }
    else // Study is stopped.
    {
        QMessageBox msgBox;
        msgBox.setText("Study was stopped. It is impossible to add further patients.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    ui->gB_response->repaint();
}


void StudyProgressPage::addPatientToTable(Study::Patient p)
{
    QTableWidgetItem *item;
    ui->t_EnrolledPatients->setRowCount(ui->t_EnrolledPatients->rowCount() +1);

    item = new QTableWidgetItem(p.patID);
    ui->t_EnrolledPatients->setItem(ui->t_EnrolledPatients->rowCount() -1, 0, item);

    if(p.response)
        item = new QTableWidgetItem("Yes");
    else
        item = new QTableWidgetItem("No");
    ui->t_EnrolledPatients->setItem(ui->t_EnrolledPatients->rowCount() -1, 1, item);

    if(!(currentStudy->getIsSimondesign()))
    {
        //Display second endpoint.
        if(p.response_ep2)
            item = new QTableWidgetItem("Yes");
        else
            item = new QTableWidgetItem("No");
        ui->t_EnrolledPatients->setItem(ui->t_EnrolledPatients->rowCount() -1, 2, item);
    }

    ui->pB_removePat->setEnabled(true);
}

void StudyProgressPage::updateDetails()
{
    QString s;
    QTextStream ts(&s, QIODevice::ReadWrite);
    ts.setRealNumberPrecision(2);

    ts << "<b>Enrolled Patients</b>: " << currentStudy->getEnrolledPatients()->size();
    ui->l_enrolledPats->setText(s);
    s.clear();
    if(currentStudy->getIsSimondesign())
    {
        Result *res = currentStudy->getSolutionStrategy();
        ts << "<b>Responses</b>: " << currentStudy->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();
        if(simon == NULL)
        {
            simon = new R_simondesign();
            simon->loadPackage();
        }

        ts << "<b>Conditional Power</b>: " << simon->getConditionalPower(currentStudy->getNumOfResponses(), currentStudy->getEnrolledPatients()->size(), res->r1, res->n1, res->r, res->n, res->p1);
        ui->l_cp->setText(s);
        s.clear();
    }
    else
    {
        Sub1_result *res = currentStudy->getSolutionStrategy_Sub1();
        ts << "<b>Responses endpoint one</b>: " << currentStudy->getNumOfResponses();
        ui->l_responses->setText(s);
        s.clear();
        ts << "<b>Responses endpoint two</b>: " << currentStudy->getNumOfResponses_ep2();
        ui->l_responses_ep2->setText(s);
        s.clear();
        if(sub1 == NULL)
        {
            sub1 = new R_Sub1Design();
            sub1->loadPackage();
        }

        ts << "<b>Conditional power</b>: " << sub1->get_conditionalPower(currentStudy->getNumOfResponses(), currentStudy->getNumOfResponses_ep2()
                                                                         ,currentStudy->getEnrolledPatients()->size(), res->getR1(), res->getN1(), res->getR(), res->getS(), res->getN(), res->getPc1(), res->getPt1());
        ui->l_cp->setText(s);
        s.clear();
    }

}

void StudyProgressPage::on_lE_patID_textChanged(const QString &arg1)
{
    // Only enable the "AddPatient"-button if a patient ID is entered.
    ui->pB_AddPatient->setEnabled(!(ui->lE_patID->text().isEmpty())
                                  && (ui->rb_noResponse->isChecked() || ui->rb_response->isChecked()));
}

void StudyProgressPage::on_sB_flex_n_valueChanged(int arg1)
{
    if(currentStudy->getIsSimondesign())
    {
        if(!flex_cp_changed && !flex_n_changed)
        {
            flex_n_changed = true;
            Result *res = currentStudy->getSolutionStrategy();
            ui->dsb_flex_cp->setValue(simon->getCP_flex(arg1 - res->n1, res, currentStudy->getNumOfResponses(), currentStudy->getAlphaNominal()));

            std::map<int, double> *flexValues = NULL;
            std::map<int, double>::iterator it;
            switch(res->flexSol)
            {
            case Result::Proportional:
                flexValues = res->flexibleAdaptionProp;
                break;
            case Result::Equal:
                flexValues = res->flexibleAdaptionEqua;
                break;
            case Result::ToOne:
                flexValues = res->flexibleAdaptionToOne;
                break;
            case Result::None:
                flexValues = res->flexibleAdaptionNone;
                break;
            default:
                break;
            }

            it = flexValues->find(currentStudy->getNumOfResponses());
            double ce = it->second;
            int r2 = 0;
            r2 = simon->getR2forFlex(ce, res->p0, arg1 - res->n1);

            QString new_r;
            QTextStream ts(&new_r, QIODevice::ReadWrite);
            tmp_r_flex = res->r1 + r2;
            ts << "<b>R</b> flexieble: " << (tmp_r_flex);
            ui->l_r_flex->setText(new_r);
            flex_n_changed =false;
        }
    }
}

void StudyProgressPage::on_dsb_flex_cp_valueChanged(double arg1)
{
    if(currentStudy->getIsSimondesign())
    {
        if(!flex_n_changed && !flex_cp_changed)
        {
            flex_cp_changed = true;
            Result *res = currentStudy->getSolutionStrategy();
            int new_n = simon->getN(arg1,res, currentStudy->getNumOfResponses(), currentStudy->getAlphaNominal());
            ui->sB_flex_n->setValue(new_n);

            std::map<int, double> *flexValues = NULL;
            std::map<int, double>::iterator it;
            switch(res->flexSol)
            {
            case Result::Proportional:
                flexValues = res->flexibleAdaptionProp;
                break;
            case Result::Equal:
                flexValues = res->flexibleAdaptionEqua;
                break;
            case Result::ToOne:
                flexValues = res->flexibleAdaptionToOne;
                break;
            case Result::None:
                flexValues = res->flexibleAdaptionNone;
                break;
            default:
                break;
            }

            it = flexValues->find(currentStudy->getNumOfResponses());
            double ce = it->second;
            int r2 = 0;
            r2 = simon->getR2forFlex(ce, res->p0, new_n - res->n1);

            QString new_r;
            QTextStream ts(&new_r, QIODevice::ReadWrite);
            tmp_r_flex = res->r1 + r2;
            ts << "<b>R flexieble</b>: " << (tmp_r_flex);
            ui->l_r_flex->setText(new_r);


            flex_cp_changed = false;
        }
    }
}

void StudyProgressPage::on_pB_set_Flex_values_clicked()
{
    // Save changes to the study design (based on the flexible extension).
    if(currentStudy->getIsSimondesign())
    {
        this->currentStudy->getSolutionStrategy()->n = ui->sB_flex_n->value();
        this->currentStudy->getSolutionStrategy()->r = tmp_r_flex;
        ui->w_Rplot->setSimonStoppingRulesSet(false);
        this->showStudyDetails(this->currentStudy);
    }
}


void StudyProgressPage::on_rb_response_clicked(bool checked)
{
    if(currentStudy->getIsSimondesign())
        ui->pB_AddPatient->setEnabled( !(ui->lE_patID->text().isEmpty())
                                      && (ui->rb_noResponse->isChecked() || ui->rb_response->isChecked()));
    else
    {
        ui->pB_AddPatient->setEnabled(!(ui->lE_patID->text().isEmpty())
                                      && (ui->rb_noResponse->isChecked() || (ui->rb_response->isChecked() && ui->cB_ep2->isChecked() )));
    }
}

void StudyProgressPage::on_rb_noResponse_clicked(bool checked)
{
    if(currentStudy->getIsSimondesign())
        ui->pB_AddPatient->setEnabled( !(ui->lE_patID->text().isEmpty())
                                      && (ui->rb_noResponse->isChecked() || ui->rb_response->isChecked()));
    else
    {
        ui->pB_AddPatient->setEnabled(!(ui->lE_patID->text().isEmpty())
                                      && (ui->rb_noResponse->isChecked() || (ui->rb_response->isChecked() && ui->cB_ep2->isChecked() )));
    }
}

void StudyProgressPage::on_pB_removePat_clicked()
{
    if(currentStudy->removePatient())
    {
        ui->t_EnrolledPatients->removeRow(ui->t_EnrolledPatients->rowCount()-1);
        if(ui->t_EnrolledPatients->rowCount() <= 0)
            ui->pB_removePat->setEnabled(false);

        this->updateDetails();
        currentStudy->setStopped(false);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No patient currently enrolled.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
}

void StudyProgressPage::on_cB_ep2_clicked(bool checked)
{
    if(!checked)
        ui->cB_ep1->setChecked(false);

    ui->pB_AddPatient->setEnabled(!(ui->lE_patID->text().isEmpty())
                                  && (ui->rb_noResponse->isChecked() || (ui->rb_response->isChecked() && ui->cB_ep2->isChecked() )));
}

void StudyProgressPage::on_cB_ep1_clicked(bool checked)
{
    if(checked)
        ui->cB_ep2->setChecked(true);

    ui->pB_AddPatient->setEnabled(!(ui->lE_patID->text().isEmpty())
                                  && (ui->rb_noResponse->isChecked() || (ui->rb_response->isChecked() && ui->cB_ep2->isChecked() )));
}
