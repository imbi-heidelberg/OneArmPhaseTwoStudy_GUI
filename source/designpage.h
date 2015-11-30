// An object of this class represents the second page / form
// which has to be filled out in the planning phase of a study.
// On this page all necessary calculations and choices can be made to select a
// proper design for a given study-situation.
//
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


#ifndef DESIGNPAGE_H
#define DESIGNPAGE_H

#include <QWidget>
#include "result.h"
#include "sub1_result.h"
#include <vector>
#include <map>
#include <time.h>
#include <tr1/tuple>
#include "r_simondesign.h"
#include "r_sub1design.h"
#include <math.h>
#include <QString>
#include <QMenu>
#include "idtablewidgetitem.h"

namespace Ui {
    class DesignPage;
}

class DesignPage : public QWidget
{
    Q_OBJECT

public:
    // Standart constructor.
    explicit DesignPage(QWidget *parent = 0);
    // Standart descructor.
    ~DesignPage();

    // Reset the view of the "DesignPage" and repaint it.
    void reset();

    // Returns the "Result"-object which stores the design parameters / informations, selected by the user. (Simon's design was calculated)
    Result* getSelectedSolution();
    // Returns the "Sub1_result"-object which stores the design parameters / informations, selected by the user. (Subset design was calculated)
    Sub1_result* getSelectedSolution_Sub1();

    // Returns the cut in percent which wis chosen by the user for a given selected design.
    // If no cut is selected or curtailment is not used at all "-1" is returned.
    int getSelectedCut();

    // Returns the nominal alpha level provided by the user.
    double getAlphaNominal();

    // Returns "true" if "Simon's Design" is selectet for design calculation
    // Returns "false" if "Sub1 Design" is selectet for design calculation
    bool simonSelected();

signals:
    // Signal to inform the "MainWindow"-object whether or not the "next"-button
    // should be enabled. (If "ready" == "true" enable "next"-button else disable the button)
    void readyToProceed(bool ready);

public slots:
    // Based on the selections made by the user informations about stochastic curtailment and/or usage of the "flexible extension"
    // (rest alpha spending developed by Stefan Englert; Heidelberg Uneversity)
    void on_showDetails();

private:
    Ui::DesignPage *ui;
    bool minimax, optimal, admissible, all;
    R_simondesign *simon;
    R_Sub1Design *sub1;
    int details_row;

    // Displays all design parameters stored in the given "Result"-object in the apropriate table. (simon's design)
    void printResult(Result* r);
    // Displays all stochastical curtailment informations stored in the given "Result"-object in the apropriate table. (simon's design)
    void printResultSC(Result* r);

    // Displays all design parameters stored in the given "Sub1_result"-object in the apropriate table. (subset design)
    void printResult_Sub1(Sub1_result *r);
    // Displays all stochastical curtailment informations stored in the given "Sub1_result"-object in the apropriate table. (subset design)
    void printResultSC(Sub1_result *r);
    // Checks every selection and input the user made on the designpage and returns true if all nessesary
    // inputs/selections are made to select a design for the study. Otherwise false is returned.
    bool validateUserInput();

private slots:
    // The following slots handel all signals issued by the gui which are needed to calculate and select
    // a propper design for the given study-situation.

    // Handels the click signal from the "stochastical curtailment"-table.
    // (Selects the row into which the user clicked.)
    void on_t_ScDetails_itemClicked(QTableWidgetItem* item);
    // Enables/disables the gui elements needed to specify how to calculate stochastical curtailment.
    void on_cB_Curtailment_toggled(bool checked);
    // Display a custom context menu at the current cursor position
    // (Context menu enables the user to display details of a givn design.)
    void on_t_results_customContextMenuRequested(QPoint pos);
    // Starts the algorithm which searches for designs which fit into the selections the user has provided.
    void on_pB_StartCalc_clicked();
    // Sets the "all" member variable to the value of "checked"
    // (Display all identified designs after calculation)
    void on_rB_All_toggled(bool checked);
    // Sets the "admissible" member variable to the value of "checked"
    // (Display only admissible designs among the identified designs after calculation)
    void on_rB_Admissible_toggled(bool checked);
    // Sets the "optimal" member variable to the value of "checked"
    // (Only display the optimal design among the identified designs after calculation)
    void on_rB_Optimal_toggled(bool checked);
    // Sets the "minimax" member variable to the value of "checked"
    // (Only display the minimax design among the identified designs after calculation)
    void on_rB_Minimax_toggled(bool checked);
    // Handels the click signal from the "results"-table (which displays all identified designs).
    // (Selects the row into which the user clicked.)
    void on_t_results_itemClicked(QTableWidgetItem* item);
    // Trigers the calculation of the "maxN"-value
    //(determines the maximal over all patient count for the design identification)
    void on_b_CalcMaxN_clicked();
    // Disables the "start calculation"-button if "ui->sb_MaxN->value()" is < 1
    // Enables the button otherwise.
    void on_sb_MaxN_valueChanged(int );
    // Enables the "alpha spending selection area" in the gui if "arg1 == Qt::Checked" (otherwise disables the area).
    void on_cB_flexible_stateChanged(int arg1);
    void on_dsb_alpha_valueChanged(double arg1);
    void on_dsb_beta_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "p0", "maxN" has to be recalculated.
    void on_dsb_p0_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "p1", "maxN" has to be recalculated.
    void on_dsb_p1_valueChanged(double arg1);
    // Displays the gui elements which are needed to calculate subset designs.
    void on_rB_sub1_toggled(bool checked);
    // Displays the gui elements which are needed to calculate simon's two stage designs.
    void on_rB_simon_toggled(bool checked);
    void on_sb_MaxN_sub1_valueChanged(int arg1);
    void on_dsb_alpha_sub1_valueChanged(double arg1);
    void on_dsb_beta_sub1_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "pc0", "maxN" has to be recalculated.
    void on_dsp_pc0_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "pt0", "maxN" has to be recalculated.
    // Also checks that pt0 is < pc0
    void on_dsp_pt0_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "pc1", "maxN" has to be recalculated.
    // Also checks that pt1 is < pc1
    void on_dsp_pc1_valueChanged(double arg1);
    // Sets "maxN" to "0". Therfore after changing "pt1", "maxN" has to be recalculated.
    // Also checks that pt1 is < pc1
    void on_dsp_pt1_valueChanged(double arg1);
    void on_sb_minN_sub1_valueChanged(int arg1);
    void on_cb_skipN1_clicked(bool checked);
};

#endif // DESIGNPAGE_H
