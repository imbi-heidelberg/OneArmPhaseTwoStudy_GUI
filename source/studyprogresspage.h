// An object of this class provides the functionality to display and monitor the enrollment phase
// of a study (including the interim analysis and add/remove patients to the study).
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

#ifndef STUDYPROGRESSPAGE_H
#define STUDYPROGRESSPAGE_H

#include <QWidget>
#include "study.h"
#include <QMessageBox>
#include <QPen>
#include "r_simondesign.h"
#include "r_sub1design.h"

namespace Ui {
    class StudyProgressPage;
}

class StudyProgressPage : public QWidget
{
    Q_OBJECT

public:
    explicit StudyProgressPage(QWidget *parent = 0);
    ~StudyProgressPage();
    // Sets the study to monitor.
    void setStudy(Study *study);

signals:
    // Signal to inform the mainwindow that the current study has finished ("n" patients are enrolled).
    void studyFinished(bool finished);

private:
    // Adds a patient to the apropriate table (in the view) and emits the "studyFinished" signal if "n" patients are enrolled.
    void addPatientToTable(Study::Patient p);
    // Displays details (like the choosen design, number of enrolled patients, number of response, etc.)
    // about the current study on the corresponding QTabWidget.
    void showStudyDetails(Study *study);
    // Similar functionality like "showStudyDetails" but only updates the displayed informations about
    // "Enrolled Patients", "Responses" and "Conditional Power".
    void updateDetails();

    Ui::StudyProgressPage *ui;
    Study *currentStudy;
    // Needed for "condidional power" calculation.
    R_simondesign *simon;
    // Needed for "condidional power" calculation.
    R_Sub1Design *sub1;

    bool flex_n_changed;
    bool flex_cp_changed;
    int tmp_r_flex;

private slots:
    // The following slots handle the click events from the GUI.
    void on_pB_AddPatient_clicked();
    void on_sB_flex_n_valueChanged(int arg1);
    void on_dsb_flex_cp_valueChanged(double arg1);
    void on_pB_set_Flex_values_clicked();
    void on_lE_patID_textChanged(const QString &arg1);
    void on_rb_response_clicked(bool checked);
    void on_rb_noResponse_clicked(bool checked);
    void on_pB_removePat_clicked();
    void on_cB_ep2_clicked(bool checked); // If
    void on_cB_ep1_clicked(bool checked);
};

#endif // STUDYPROGRESSPAGE_H
