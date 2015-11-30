// An object of this class represents the top level of the GUI.
// Any information displayed for planning, monitoring and analysing studies is
// controlled by this class.
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "result.h"
#include "study.h"
#include <QFileDialog>
#include <QMessageBox>
#include "r_simondesign.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Default constructor
    explicit MainWindow(QWidget *parent = 0);
    // Default desturctor
    ~MainWindow();

private:

    enum Page {MAIN_INFO, STUDY_SOLUTION, OVERVIEW, STUDY_PROGRESS, STUDY_RESULTS};

    Ui::MainWindow *ui;
    // Redraws the mainwindow and displays the content which fits the
    // "pageCounter" argument.
    //
    // "pageCounter" == MAIN_INFO => display a widget of class "NewStudyFirstPage"
    // "pageCounter" == STUDY_SOLUTION => display a widget of class "DesignPage"
    // "pageCounter" == OVERVIEW => display a widget of class "NewStudyOverviewPage"
    // "pageCounter" == STUDY_PROGRESS => display a widget of class "StudyProgressPage"
    // "pageCounter" == STUDY_RESULTS => display a widget of class "StudyEvaluationPage"
    void updateView(Page pageCounter);
    // Private enum variable which determines which content should be displayed (passed to the updateView method).
    Page pageCounter;
    // Contains the study object (either of the study which is to be planned or a study loaded from file)
    Study *study;
    // This variable is used in the planning phase of a study and determines if the user can hit the "next"-button
    // to proceed to the next page (NewStudyFirstPage -> DesignPage -> NewStudyOverviewPage)
    bool readyToProceed;
    // Contains the path to the serialized "Study"-object
    QString studyPath;

private slots:
    // This function lets the user choose a file and tries to load a "Study"-object from that file.
    void on_OpenStudy();
    // Write the "study" member to the file from which it was loaded.
    void on_SaveStudy();
    // Write the "study" member to a file choosen by the user.
    void on_actionSave_as_triggered();
    // Sets the member "readyToProceed" to the given value and issues the "updateView" method
    void on_ReadyToProceed(bool readyToProceed);
    // This method is used in the planning phase of a study. Depending of the value of the "pageCounter" member
    // the value of this member is changed and afterwards the "updateView" method is issued. The value of "pageCounter
    // is changed according to the rules below:
    // STUDY_SOLUTION => MAIN_INFO
    // OVERVIEW => SUTDY_SOLUTION
    // MAIN_INFO => MAIN_INFO
    void on_pB_Back_clicked();
    // This method is used in the planning phase of a study. Depending of the value of the "pageCounter" member
    // the value of this member is changed and afterwards the "updateView" method is issued. The value of "pageCounter
    // is changed according to the rules below:
    // MAIN_INFO => STUDY_SOLUTION
    // STUDY_SOLUTION => OVERVIEW
    // OVERVIEW => OVERVIEW
    void on_pB_Next_clicked();
    // Deletes the current "Study" object and creates a new one. Also sets the "pageCounter" to MAIN_INFO and issues the "updateView" method.
    void on_CreateNewStudy();
    // Closes the program.
    void on_actionQuit_triggered();
    // If "finished" == true the "pageCounter" is set to STUDY_RESULTS and "updateView" is invoked.
    // if "finished" == false nothing happens.
    void on_studyFinished(bool finished);    
};

#endif // MAINWINDOW_H
