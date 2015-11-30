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

#include "designpage.h"
#include "ui_designpage.h"

DesignPage::DesignPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DesignPage)
{
    ui->setupUi(this);
    minimax = false;
    optimal = false;
    admissible = false;
    all = true;
    details_row = -1;
    // Hide all gui elements which are not initial needed.
    ui->tab_Details->hide();
    ui->tab_flexible->hide();
    ui->gB_sub1_para->hide();

    // Ids of the identified designs are not displayed for the user. Therefore column 8 is hidden.
    ui->t_results->hideColumn(8);
    ui->t_results->setContextMenuPolicy(Qt::CustomContextMenu);

    // Create objects needed to calculate "simon's two stage designs" and "subset designs"
    simon = new R_simondesign();
    sub1 = new R_Sub1Design();


    // Set tooltips for the gui elements
    ui->b_CalcMaxN->setToolTip("Calculate the aproximal needed number of patients for the optimal design.");
    ui->cB_curtailAll->setToolTip("Calculate the influence of curtailment from the given start value till one.");
    ui->cB_Curtailment->setToolTip("Should curtailment be used in the study?");
    ui->cB_flexible->setToolTip("Should the flexible extension be used?");
    ui->dsb_alpha->setToolTip("Maximal type one error.");
    ui->dsb_beta->setToolTip("Maximal type two error.");
    ui->dsb_Curtailment->setToolTip("Cut value fot the curtailment.\n(A cut-value of zero equalls the non stochastical curtailment.)");
    ui->dsb_p0->setToolTip("Probability for an event under the assumption the zero hypothesis holds true.");
    ui->dsb_p1->setToolTip("Probability for an event under the assumption the alternative hypothesis holds true.");
    ui->pB_StartCalc->setToolTip("Start calculation of possilbe designs.");
    ui->rB_Admissible->setToolTip("Only show \"admissible\" desings.");
    ui->rB_Minimax->setToolTip("Only show the \"minimax\" desing");
    ui->rB_Optimal->setToolTip("Only show the \"optimal\" design");

    // Enable to hide tabs.
    ui->tab_Details->setStyleSheet("QTabWidget::tab:disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }");
    ui->tab_Details->setStyleSheet("QTabBar::tab:disabled { width: 0; color: transparent; background: transparent; }");
}

DesignPage::~DesignPage()
{
    delete ui;
    delete simon;
    delete sub1;
}

void DesignPage::on_sb_MaxN_valueChanged(int )
{
    if(ui->rB_simon->isChecked())
    {
        if(ui->sb_MaxN->value() > 1)
        {
            ui->pB_StartCalc->setEnabled(true);
        }
        else
        {
            ui->pB_StartCalc->setEnabled(false);
        }
    }
}

void DesignPage::on_b_CalcMaxN_clicked()
{
    int maxn = 0;

    // Is "simon's design" selected for calculation?
    if(ui->rB_simon->isChecked())
    {
        // Calculate maxN for the "simon's design"
        simon->setAlpha(ui->dsb_alpha->value());
        simon->setBeta(ui->dsb_beta->value());
        simon->setP0(ui->dsb_p0->value());
        simon->setP1(ui->dsb_p1->value());
        maxn = simon->aproximateMaxN();
        ui->sb_MaxN->setValue(maxn);
    }
    else if(ui->rB_sub1->isChecked())
    {
        // Calculate maxN for the "subset design"
        sub1->setAlpha(ui->dsb_alpha_sub1->value());
        sub1->setBeta(ui->dsb_beta_sub1->value());
        sub1->setPc0(ui->dsp_pc0->value());
        sub1->setPt0(ui->dsp_pt0->value());
        sub1->setPc1(ui->dsp_pc1->value());
        sub1->setPt1(ui->dsp_pt1->value());
        maxn = sub1->aproximateMaxN();
        ui->sb_MaxN_sub1->setValue(maxn);
    }


}

void DesignPage::on_t_results_itemClicked(QTableWidgetItem* item)
{

    ui->t_results->selectRow(item->row());

    // If a new design got selected clear the details content.
    if(details_row != item->row()){
        ui->t_ScDetails->clearContents();
        ui->t_ScDetails->setRowCount(0);
    }
    details_row = item->row();
    emit readyToProceed(validateUserInput());
}

void DesignPage::on_rB_Minimax_toggled(bool checked)
{
    minimax = checked;
}

void DesignPage::on_rB_Optimal_toggled(bool checked)
{
    optimal = checked;
}

void DesignPage::on_rB_Admissible_toggled(bool checked)
{
    admissible = checked;
}

void DesignPage::on_rB_All_toggled(bool checked)
{
    all = checked;
}

void DesignPage::on_pB_StartCalc_clicked()
{
    //Clear all contents.
    reset();


    ui->t_results->setSortingEnabled(false);
    // Which design type is selected?
    if(ui->rB_simon->isChecked())
    {
        // Set all necessary values for "simon's two stage design" calculation
        simon->setAlpha(ui->dsb_alpha->value());
        simon->setBeta(ui->dsb_beta->value());
        simon->setP0(ui->dsb_p0->value());
        simon->setP1(ui->dsb_p1->value());
        simon->setMaxN(ui->sb_MaxN->value());

        // Loesungen berechnen
        // Start design calculation/identification
        simon->calculateStudySolutions();

        // Display identified designs
        int i, numbOfRes;
        numbOfRes = simon->getNumberOfResults();
        if(numbOfRes > 0)
        {
            if(!all)// Do not display all identified designs.
            {
                if(minimax)
                {
                    printResult(simon->getResult(simon->getMiniMaxPos()));
                }

                if(optimal )
                {
                    printResult(simon->getResult(simon->getOptimalPos()));
                }

                if(admissible)
                {
                    for(i=0; i< numbOfRes; i++)
                    //if(i!= miniMaxPos && i != optimalPos)
                         if(simon->getResult(i)->admissible)
                         {
                             printResult(simon->getResult(i));
                         }
                }
            }
            else // Display all identified designs.
            {
                for(i=0; i< numbOfRes; i++)
                    printResult(simon->getResult(i));//
            }
        }
        else // No design identified.
        {
            QMessageBox msgBox;
            msgBox.setText("No designs found for the given maxN.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        ui->t_results->horizontalHeader()->setSortIndicatorShown(true);
        ui->t_results->horizontalHeader()->setSortIndicator(8, Qt::AscendingOrder);
    }
    else if(ui->rB_sub1->isChecked())
    {
        // Set all necessary values for "subset design" calculation
        sub1->setAlpha(ui->dsb_alpha_sub1->value());
        sub1->setBeta(ui->dsb_beta_sub1->value());
        sub1->setPc0(ui->dsp_pc0->value());
        sub1->setPt0(ui->dsp_pt0->value());
        sub1->setPc1(ui->dsp_pc1->value());
        sub1->setPt1(ui->dsp_pt1->value());

        sub1->calculateStudySolutions(ui->cB_skipS->isChecked(), ui->cb_skipR->isChecked(), ui->cb_skipN1->isChecked(), ui->sb_minN_sub1->value(), ui->sb_MaxN_sub1->value());

        // Display identified designs
        int i, numbOfRes;
        numbOfRes = sub1->getNumberOfResults();
        if(numbOfRes > 0)
        {
            if(!all) // Do not display all identified designs.
            {
                if(minimax)
                {
                    printResult_Sub1(sub1->getResult(sub1->getMinimaxPos()));
                }
                else if(optimal)
                {
                    printResult_Sub1(sub1->getResult(sub1->getOptimalPos()));
                }
                else
                {
                    for(i=0; i< numbOfRes; i++)
                         if(sub1->getResult(i)->getAdmissible())
                         {
                             printResult_Sub1(sub1->getResult(i));
                         }
                }
            }
            else // Display all identified designs.
            {
                for(i=0; i< numbOfRes; i++)
                    printResult_Sub1(sub1->getResult(i));
            }

        }
        else // No design identified.
        {
            QMessageBox msgBox;
            msgBox.setText("No designs found for the given minN and maxN.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        ui->t_results->horizontalHeader()->setSortIndicatorShown(true);
        ui->t_results->horizontalHeader()->setSortIndicator(8, Qt::AscendingOrder);

    }


    ui->t_results->setSortingEnabled(true);
}


void DesignPage::on_t_results_customContextMenuRequested(QPoint pos)
{
    // Only display the "details" context menu if one of the design extensions is selected.
    if(ui->cB_Curtailment->isChecked() || ui->cB_flexible->isChecked())
    {
        QMenu menu(ui->t_results);
        QAction *action = menu.addAction("Show details");
        QObject::connect(action, SIGNAL(triggered()), this, SLOT(on_showDetails()));
        menu.exec(QCursor::pos());
    }
}

void DesignPage::on_showDetails()
{
    //Get the ID of the selected design.
    QString iD;
    if(ui->t_results->selectedItems().size() == 0)
    {
     iD = "0";
    }
    else
    {
        iD = ui->t_results->item(ui->t_results->selectedItems()[0]->row(),8)->text();
    }

    // Calculate the influence of stochastic curtailment for the selected design
    if(ui->cB_Curtailment->isChecked())
    {
        if(ui->rB_simon->isChecked())
        {
            simon->calculateSC(iD.toInt(), ui->dsb_Curtailment->value(), ui->sb_NumRep->value(), ui->cB_curtailAll->isChecked());
        }
        else
        {
            sub1->calculateSC(iD.toInt(), ui->dsb_Curtailment->value(), ui->sb_NumRep->value(), ui->cB_curtailAll->isChecked());
        }

    }
    if(ui->cB_flexible->isChecked() & !(ui->rB_sub1->isChecked()))
    {
        simon->calcultateCE(iD.toInt());
    }


    // Get the selected design
    Result* res;
    Sub1_result *res_sub1;
    if(ui->rB_simon->isChecked())
    {
        res =  simon->getResult(iD.toInt());
        res_sub1 = NULL;
    }
    else
    {
        res_sub1 = sub1->getResult(iD.toInt());
        res = NULL;
    }
    ui->tab_Details->show();

    // Display curtailment details (if selected).
    if(ui->cB_Curtailment->isChecked())
    {        
        ui->tab_Details->setTabEnabled(0, true);
        ui->t_ScDetails->clearContents();
        ui->t_ScDetails->setRowCount(0);
        if(ui->rB_simon->isChecked())
            printResultSC(res);
        else
            printResultSC(res_sub1);
    }else // Hide tab for curtailment details.
    {
        ui->tab_Details->setTabEnabled(0, false);
    }
    if(ui->cB_flexible->isChecked())// Display flexible extension details (if selected).
    {
        ui->tab_Details->setTabEnabled(1, true);
        QString value;
        QTableWidgetItem *item;
        //int i;
        std::map<int, double>::iterator it_prop = res->flexibleAdaptionProp->begin();
        std::map<int, double>::iterator it_equa = res->flexibleAdaptionEqua->begin();
        std::map<int, double>::iterator it_toOne = res->flexibleAdaptionToOne->begin();
        std::map<int, double>::iterator it_None = res->flexibleAdaptionNone->begin();
        int k = 0;
        double ce;
        while( it_prop != res->flexibleAdaptionProp->end())
        {
            // Increase row count to display data.
            ui->t_flexible_details_proportionally->setRowCount(k +1);
            ui->t_flexible_details_equally->setRowCount(k +1);
            ui->t_flexible_details_toOne->setRowCount(k +1);
            ui->t_flexible_details_none->setRowCount(k+1);

            // Add data to the three tableWidgets
            value = QString::number(k);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_proportionally->setItem(k, 0, item);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_equally->setItem(k, 0, item);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_toOne->setItem(k, 0, item);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_none->setItem(k, 0, item);

            ce = it_prop->second;
            value.sprintf("%.4f",ce);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_proportionally->setItem(k, 1, item);

            ce = it_equa->second;
            value.sprintf("%.4f",ce);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_equally->setItem(k, 1, item);

            ce = it_toOne->second;
            value.sprintf("%.4f",ce);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_toOne->setItem(k, 1, item);

            ce = it_None->second;
            value.sprintf("%.4f",ce);
            item = new QTableWidgetItem(value);
            ui->t_flexible_details_none->setItem(k, 1, item);


            k++;
            it_prop++;
            it_equa++;
            it_toOne++;
            it_None++;
        }
    }else // Hide tab for flexible extension details.
    {
        ui->tab_Details->setTabEnabled(1, false);
    }
}

void DesignPage::on_cB_Curtailment_toggled(bool checked)
{
    // Enable gui elements for stochastic curtailment calculation (actual calculation takes place in "on_showDetails").
    ui->cB_curtailAll->setEnabled(checked);
    ui->dsb_Curtailment->setEnabled(checked);
    ui->sb_NumRep->setEnabled(checked);

    emit readyToProceed(validateUserInput());
}

void DesignPage::printResult(Result *r)
{
    ui->t_results->setRowCount(ui->t_results->rowCount() +1);
    QString qsR, qsN;
    qsR.sprintf("%.0f",(float)r->r1);
    qsN.sprintf("%.0f",(float)r->n1);
    QTableWidgetItem *item = new QTableWidgetItem(qsR + " / " + qsN);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 0, item);
    qsR.sprintf("%.0f",(float)r->r);
    qsN.sprintf("%.0f",(float)r->n);
    item = new QTableWidgetItem(qsR + " / " + qsN);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 1, item);
    qsR.sprintf("%.4f",r->enP0);
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 2, item);
    qsR.sprintf("%.4f",r->petP0);
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 3, item);
    qsR.sprintf("%.4f",r->alpha);
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 4, item);
    qsR.sprintf("%.4f",r->beta);
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 5, item);
    if(r->admissible)
    {
        qsR.sprintf("%.2f",(float)r->admissibleStart);
        qsN.sprintf("%.2f",(float)r->admissibleStop);
        item = new QTableWidgetItem(qsR + "<-->" + qsN);
        ui->t_results->setItem(ui->t_results->rowCount()-1, 6, item);
        item = new QTableWidgetItem(*(r->name));
        ui->t_results->setItem(ui->t_results->rowCount()-1, 7, item);
    }
    else
    {
        item = new QTableWidgetItem("");
        ui->t_results->setItem(ui->t_results->rowCount()-1, 6, item);
    }

    // Store the design-id in the hidden column which is used to identify the design selected by the user
    qsR.sprintf("%.0f",(float)r->iD);
    item = new IdTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 8, item);
}

void DesignPage::printResult_Sub1(Sub1_result *r)
{
    ui->t_results->setRowCount(ui->t_results->rowCount() +1);
    QString qsR, qsN, qsS;
    qsR.sprintf("%.0f",(float)r->getR1());
    qsN.sprintf("%.0f",(float)r->getN1());
    QTableWidgetItem *item = new QTableWidgetItem(qsR + " / " + qsN);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 0, item);
    qsR.sprintf("%.0f",(float)r->getR());
    qsS.sprintf("%.0f",(float)r->getS());
    qsN.sprintf("%.0f",(float)r->getN());
    item = new QTableWidgetItem(qsR + "," + qsS + " / " + qsN);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 1, item);
    qsR.sprintf("%.4f",r->getEnP0());
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 2, item);
    qsR.sprintf("%.4f",r->getPetP0());
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 3, item);
    qsR.sprintf("%.4f",r->getAlpha());
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 4, item);
    qsR.sprintf("%.4f",r->getBeta());
    item = new QTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 5, item);
    if(r->getAdmissible())
    {
        qsR.sprintf("%.2f",(float)r->getAdmissibleStart());
        qsN.sprintf("%.2f",(float)r->getAdmissibleStop());
        item = new QTableWidgetItem(qsR + "<-->" + qsN);
        ui->t_results->setItem(ui->t_results->rowCount()-1, 6, item);
        item = new QTableWidgetItem(*(r->getName()));
        ui->t_results->setItem(ui->t_results->rowCount()-1, 7, item);
    }
    else
    {
        item = new QTableWidgetItem("");
        ui->t_results->setItem(ui->t_results->rowCount()-1, 6, item);
    }
    // Store the design-id in the hidden column which is used to identify the design selected by the user
    qsR.sprintf("%.0f",(float)r->getID());
    item = new IdTableWidgetItem(qsR);
    ui->t_results->setItem(ui->t_results->rowCount()-1, 8, item);
}

void DesignPage::printResultSC(Result *r)
{
    QString value;
    QTableWidgetItem *item;

    std::map<int, Result::Curtailment>::iterator it = r->curtailmentResults->begin();
    // Display all calculated stochastic curtailment information.
    while( it != r->curtailmentResults->end())
    {
        ui->t_ScDetails->setRowCount(ui->t_ScDetails->rowCount() +1);
        Result::Curtailment c = it->second;
        value.sprintf("%.2f",c.cut);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 0, item);
        value.sprintf("%.4f",c.en_sc);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 1, item);
        value.sprintf("%.4f",c.pet_sc);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 2, item);
        value.sprintf("%.4f",c.type1_errorRate);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 3, item);
        value.sprintf("%.4f",c.type2_errorRate);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 4, item);
        it++;
    }
    ui->t_ScDetails->repaint();
}

void DesignPage::printResultSC(Sub1_result *r)
{
    QString value;
    QTableWidgetItem *item;

    std::map<int, Sub1_result::Curtailment_SubD1>::iterator it = r->curtailmentResults->begin();
    // Display all calculated stochastic curtailment information.
    while( it != r->curtailmentResults->end())
    {
        ui->t_ScDetails->setRowCount(ui->t_ScDetails->rowCount() +1);
        Sub1_result::Curtailment_SubD1 c = it->second;//r->curtailmentResultss->at(i);
        value.sprintf("%.2f",c.cut);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 0, item);
        value.sprintf("%.4f",c.en_sc);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 1, item);
        value.sprintf("%.4f",c.pet_sc);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 2, item);
        value.sprintf("%.4f",c.type1_errorRate);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 3, item);
        value.sprintf("%.4f",c.type2_errorRate);
        item = new QTableWidgetItem(value);
        ui->t_ScDetails->setItem(ui->t_ScDetails->rowCount()-1, 4, item);
        it++;
    }
    ui->t_ScDetails->repaint();
}

void DesignPage::reset()
{
    ui->t_results->clearContents();
    ui->t_results->setRowCount(0);
    ui->tab_Details->hide();
    this->repaint();
    // Process events to ensure that the "DesignPage"-object is repainted immediately.
    QApplication::processEvents();
}

Result* DesignPage::getSelectedSolution()
{
    //Get the ID of the selected design.
    QString iD;
    if(ui->t_results->selectedItems().size() == 0)
    {
     iD = "0";
    }
    else
    {
        iD = ui->t_results->item(ui->t_results->selectedItems()[0]->row(),8)->text();
    }

    // Little hack to ensure that all necessary calculations for the flexible extension are done when the usere presses the "next" button.
    if(ui->cB_flexible->isChecked())
        simon->calcultateCE(iD.toInt());

    Result * res = simon->getResult(iD.toInt());

    // Little hack to ensure that stochastic curtailment is calculated for the selected cut (before proceeding to the next page).
    if(ui->cB_Curtailment->isChecked())
    {
        res->useCurtailment = true;
        res->cut = getSelectedCut();
        if(!ui->cB_curtailAll->isChecked())
        {
            std::map<int, Result::Curtailment>::iterator it = res->curtailmentResults->find(getSelectedCut());
            if(it == res->curtailmentResults->end())
                simon->calculateSC(iD.toInt(), ui->dsb_Curtailment->value(), ui->sb_NumRep->value(), ui->cB_curtailAll->isChecked());
        }
    }
    else
        res->useCurtailment = false;

    if(ui->cB_flexible->isChecked())
    {
        // Store the "alpha spending" method selected for the "flexible extension".
        res->useFlexible = true;
        if(ui->rb_proportionally->isChecked())
            res->flexSol = Result::Proportional;
        else if(ui->rb_eually->isChecked())
            res->flexSol = Result::Equal;
        else if(ui->rb_smallest->isChecked())
            res->flexSol = Result::ToOne;
        else if(ui->rb_none->isChecked())
            res->flexSol = Result::None;
    }
    else
        res->useFlexible = false;

    return res;
}

Sub1_result *DesignPage::getSelectedSolution_Sub1()
{
    ///Get the ID of the selected design.
    QString iD;
    if(ui->t_results->selectedItems().size() == 0)
    {
     iD = "0";
    }
    else
    {
        iD = ui->t_results->item(ui->t_results->selectedItems()[0]->row(),8)->text();
    }

    Sub1_result * res = sub1->getResult(iD.toInt());

    // Little hack to ensure that stochastic curtailment is calculated for the selected cut (before proceeding to the next page).
    if(ui->cB_Curtailment->isChecked())
    {
        res->setUseCurtailment(true);
        res->setCut(getSelectedCut());
        if(!ui->cB_curtailAll->isChecked())
        {
            std::map<int, Sub1_result::Curtailment_SubD1>::iterator it = res->getCurtailmentResults()->find(getSelectedCut());
            if(it == res->getCurtailmentResults()->end())
                sub1->calculateSC(iD.toInt(), ui->dsb_Curtailment->value(), ui->sb_NumRep->value(), ui->cB_curtailAll->isChecked());
        }
    }
    else
        res->setUseCurtailment(false);

    return res;
}

int DesignPage::getSelectedCut()
{
    // Is the usage of stochastic curtailment selected?
    if(ui->cB_Curtailment->isChecked())
    {
        // Are there curtailment influences calculated for differnt "cut" values?
        if(ui->cB_curtailAll->isChecked())
        {
            // Is one of the calculated curtailment influences selected?
            if(ui->t_ScDetails->selectedItems().size() > 0)
            {
                // Return selected cut in "%".
                return (int)(ui->t_ScDetails->item(ui->t_ScDetails->selectedItems().at(0)->row(), 0)->text().toFloat() * 100 + 0.5);
            }
            else
                return -1;
        }
        else
        {
            // Return selected cut in "%".
            return (int)(ui->dsb_Curtailment->value() * 100 + 0.5);
        }
    }

    return -1;
}

void DesignPage::on_t_ScDetails_itemClicked(QTableWidgetItem* item)
{
    ui->t_ScDetails->selectRow(item->row());
    emit readyToProceed(validateUserInput());
}

bool DesignPage::validateUserInput()
{
    // Did the user select a design for the given study situation?
    bool designSelected = ui->t_results->selectedItems().size() > 0;

    // If usage of stochastic curtailment is selected check if all necessary selections were made.
    bool curtailmentSelectionValid = false;
    if(ui->cB_Curtailment->isChecked())
    {
        if(ui->cB_curtailAll->isChecked())
        {
            curtailmentSelectionValid = ui->t_ScDetails->selectedItems().size() > 0;
        }
        else
            curtailmentSelectionValid = true;
    }
    else
        curtailmentSelectionValid = true;

    return designSelected && curtailmentSelectionValid;
}

void DesignPage::on_cB_flexible_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        ui->gB_distribution_Type->setEnabled(true);
    }
    else
    {
        ui->gB_distribution_Type->setEnabled(false);
    }
}

double DesignPage::getAlphaNominal()
{
    return ui->dsb_alpha->value();
}

bool DesignPage::simonSelected(){ return ui->rB_simon->isChecked();}

void DesignPage::on_dsb_alpha_valueChanged(double arg1)
{
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_dsb_beta_valueChanged(double arg1)
{
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_dsb_p0_valueChanged(double arg1)
{    
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_dsb_p1_valueChanged(double arg1)
{
    if(arg1 <= ui->dsb_p0->value())
    {
        ui->dsb_p1->setValue((ui->dsb_p0->value()+ 0.01));
    }
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_rB_sub1_toggled(bool checked)
{
    if(checked)
    {
        ui->gB_simon_para->hide();
        ui->gB_sub1_para->show();

        ui->t_results->horizontalHeaderItem(1)->setText("r,s/n");

        ui->cB_flexible->setChecked(false);
        ui->cB_flexible->setEnabled(false);
        ui->cB_flexible->setToolTip("This extension cant be used for designs with tow endpoints.");
    }
}

void DesignPage::on_rB_simon_toggled(bool checked)
{
    if(checked)
    {
        ui->gB_sub1_para->hide();
        ui->gB_simon_para->show();

        ui->t_results->horizontalHeaderItem(1)->setText("r/n");
        ui->cB_flexible->setEnabled(true);
        ui->cB_flexible->setToolTip("Should the flexible extension be used?");
    }
}

void DesignPage::on_sb_MaxN_sub1_valueChanged(int arg1)
{
    if(ui->rB_sub1->isChecked())
    {
        if(arg1 < ui->sb_minN_sub1->value())
            ui->sb_minN_sub1->setValue(arg1);

        if(ui->sb_MaxN_sub1->value() > 1)
        {
            ui->pB_StartCalc->setEnabled(true);
        }
        else
        {
            ui->pB_StartCalc->setEnabled(false);
        }
    }
}

void DesignPage::on_dsb_alpha_sub1_valueChanged(double arg1)
{
    ui->sb_MaxN_sub1->setValue(0);
}

void DesignPage::on_dsb_beta_sub1_valueChanged(double arg1)
{
    ui->sb_MaxN_sub1->setValue(0);
}

void DesignPage::on_dsp_pc0_valueChanged(double arg1)
{
    ui->sb_MaxN_sub1->setValue(0);
}

void DesignPage::on_dsp_pt0_valueChanged(double arg1)
{
    if(arg1 < ui->dsp_pc0->value())
    {
        ui->dsp_pt0->setValue(ui->dsp_pc0->value());
    }
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_dsp_pc1_valueChanged(double arg1)
{
    if(arg1 < ui->dsp_pc0->value())
    {
        ui->dsp_pc1->setValue((ui->dsp_pc0->value() + 0.01));
    }
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_dsp_pt1_valueChanged(double arg1)
{
    if(arg1 < ui->dsp_pc1->value())
    {
        ui->dsp_pt1->setValue(ui->dsp_pc1->value());
    }
    else if( arg1 <= ui->dsp_pt0->value())
    {
        ui->dsp_pt1->setValue((ui->dsp_pt0->value() + 0.01));
    }
    ui->sb_MaxN->setValue(0);
}

void DesignPage::on_sb_minN_sub1_valueChanged(int arg1)
{
    if(arg1 > ui->sb_MaxN_sub1->value())
        ui->sb_minN_sub1->setValue(ui->sb_MaxN_sub1->value());
}

void DesignPage::on_cb_skipN1_clicked(bool checked)
{
    // If the iteration over all possible "n1" values (number of patients for the first stage of the study) is skipped
    // it is impossible to identify the minimax-, optimal-, or admissible-designs
    if(checked)
    {
        ui->rB_All->setChecked(true);
        ui->rB_Minimax->setEnabled(false);
        ui->rB_Optimal->setEnabled(false);
        ui->rB_Admissible->setEnabled(false);
    }
    else
    {
        ui->rB_Minimax->setEnabled(true);
        ui->rB_Optimal->setEnabled(true);
        ui->rB_Admissible->setEnabled(true);
    }
}
