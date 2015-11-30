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

#include "mainwindow.h"
#include "ui_mainwindow.h"

//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // On program start hide all pages/widgets.
    ui->setupUi(this);
    ui->w_newSturyFirstPage->hide();
    ui->w_SolutionStrategie->hide();
    ui->w_Overview->hide();
    ui->w_WindowBottom->hide();
    ui->w_studyProgress->hide();
    ui->w_studyResults->hide();
    // Create a new "Study"-object.
    study = new Study();

    //Connect signals with the corresponding slots.
    QObject::connect(ui->actionCreate_new_Study, SIGNAL(triggered()), this, SLOT(on_CreateNewStudy()));
    QObject::connect(ui->actionShow_Details, SIGNAL(triggered()), ui->w_SolutionStrategie, SLOT(on_showDetails()));
    QObject::connect(ui->w_SolutionStrategie, SIGNAL(readyToProceed(bool)), this, SLOT(on_ReadyToProceed(bool)));
    QObject::connect(ui->w_newSturyFirstPage, SIGNAL(readyToProceed(bool)), this, SLOT(on_ReadyToProceed(bool)));
    QObject::connect(ui->actionOpen_existing_Study, SIGNAL(triggered()), this, SLOT(on_OpenStudy()));
    QObject::connect(ui->actionSave_Study, SIGNAL(triggered()), this, SLOT(on_SaveStudy()));
    QObject::connect(ui->w_studyProgress, SIGNAL(studyFinished(bool)), this, SLOT(on_studyFinished(bool)));

    // Set tooltips for the "Next"- and "Back"-button.
    ui->pB_Next->setToolTip("Proceed to next page.");
    ui->pB_Back->setToolTip("Return to last page.");
}

// Default destructor
MainWindow::~MainWindow()
{
    delete ui;
    delete study;
}

void MainWindow::on_SaveStudy()
{
    // Check if a "studyPath" is set and write "study" to that path if possible.
    if(!(this->studyPath.isEmpty()))
    {
        QFile file(this->studyPath);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
        }else{
            // Write study to file.
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_4_5);
            out << *study;
        }
        file.close();
    }
}

void MainWindow::on_actionSave_as_triggered()
{
    // Open up the file dialog to select a path/file for saving the study.
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Study"), "", tr("Study (*.stud);;AllFiles (*)"));
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
        }else{
            // Write study to file.
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_4_5);
            out << *study;
        }
        // Close the file connection and save the file path/name for late use.
        file.close();
        this->studyPath = fileName;
    }
}


void MainWindow::on_CreateNewStudy()
{
    readyToProceed = false;
    // Switch page counter to the first page in the planning phase of a study.
    pageCounter = MAIN_INFO;

    // Override the current "Study"-object with a new one.
    delete study;
    study = new Study();

    // Display the widget with the "Back" and "Next" button.
    ui->w_WindowBottom->show();
    // Reset all pages and redraw the view.
    ui->w_SolutionStrategie->reset();
    ui->w_newSturyFirstPage->reset();
    updateView(this->pageCounter);
}

// Processes the "clicked" signal from the "next" button
void MainWindow::on_pB_Next_clicked()
{
    // Depending on the current value of "pageCounter" different actions are taken.
    switch(pageCounter)
    {
    // If the first page in the planning phase of a study is displayed,
    // gather all informations like the study name and the primary investigator and store them in the "Study"-object.
    // Afterwards set the "pageCounter" to "STUDY_SOLUTION".
    case MAIN_INFO:
        study->setStudyName(ui->w_newSturyFirstPage->getStudyName());
        study->setInvestigator(ui->w_newSturyFirstPage->getInvestigator());
        study->setBiometrician(ui->w_newSturyFirstPage->getBiometrician());
        pageCounter = STUDY_SOLUTION;
        break;
    // If the page for the study design calculation is displayed (planning pahse of a study),
    // store the selected design in the "Study"-object and set the "pageCounter" to "OVERVIEW"
    case STUDY_SOLUTION:
        if(ui->w_SolutionStrategie->simonSelected())
        {
            study->setSolutionStrategy(ui->w_SolutionStrategie->getSelectedSolution());
        }
        else
        {
            study->setSolutionStrategy_Sub1(ui->w_SolutionStrategie->getSelectedSolution_Sub1());
        }
        study->setAlphaNominal(ui->w_SolutionStrategie->getAlphaNominal());
        pageCounter = OVERVIEW;
        break;
    // If the overview page for the current study is displayed (planning phase of a study),
    // save the current "Study"-object to file and set the "pageCounter" to "STUDY_PROGRESS" (begin the monitoring pahse of the study)
    case OVERVIEW:       
        on_actionSave_as_triggered();
        pageCounter = STUDY_PROGRESS;
        break;

    default:pageCounter = OVERVIEW;

    }
    readyToProceed = false;
    // Redraw the view.
    updateView(pageCounter);    
}

// Processes the "clicked" signal from the "back" button
void MainWindow::on_pB_Back_clicked()
{
    // Depending on the current value of "pageCounter" different actions are taken.
    switch(pageCounter)
    {
    // If the page for design calculation is displayed switch to the first page in the planning phase of the study.
    case STUDY_SOLUTION: pageCounter = MAIN_INFO;
        readyToProceed = true;
        break;
    // If the overview page of the study is displayed switch back to the page for design calculation.
    case OVERVIEW: pageCounter = STUDY_SOLUTION; break;
    default: pageCounter = MAIN_INFO;
    }
    // Redraw the view.
    updateView(pageCounter);
}

// Redraw the content of the GUI according to the parameter "pageCounter"
void MainWindow::updateView(Page pageCounter)
{
    switch(pageCounter)
    {
    // "pageCounter" == MAIN_INFO => display a widget of class "NewStudyFirstPage"
    // Purpose of this widget is to gather general informaitions for the planned study (name of the sutdy; investigator; biometrican).
    // If "readyToProceed" == "true" enable the "next" button else disable this button.
    case MAIN_INFO:
        ui->w_SolutionStrategie->hide();
        ui->w_Overview->hide();
        ui->w_studyProgress->hide();
        ui->w_studyResults->hide();
        ui->w_newSturyFirstPage->show();        
        ui->pB_Back->setEnabled(false);
        ui->pB_Next->setEnabled(readyToProceed);
        if(!readyToProceed)
        {
            ui->pB_Next->setToolTip("Some fields are blank.");
        }
        else
        {
            ui->pB_Next->setToolTip("Proceed to next page.");
        }
        ui->pB_Next->setText("Next");
        ui->actionShow_Details->setEnabled(false);
        ui->actionSave_Study->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
        break;
    // "pageCounter" == STUDY_SOLUTION => display a widget of class "DesignPage"
    // Purpose of this widget is the selection of a suitable design for the planned study.
    // If "readyToProceed" == "true" enable the "next" button else disable this button.
    case STUDY_SOLUTION:        
        ui->w_newSturyFirstPage->hide();
        ui->w_Overview->hide();
        ui->w_studyProgress->hide();
        ui->w_studyResults->hide();
        ui->w_SolutionStrategie->show();
        ui->pB_Back->setEnabled(true);
        ui->pB_Next->setEnabled(readyToProceed);
        if(!readyToProceed)
        {
            ui->pB_Next->setToolTip("No design and/or cut for curtailment (if used) chosen.");
        }
        else
        {
            ui->pB_Next->setToolTip("Proceed to next page.");
        }
        ui->pB_Next->setText("Next");
        ui->actionSave_Study->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
        break;
    // "pageCounter" == OVERVIEW => display a widget of class "NewStudyOverviewPage"
    // Purpose of this widget is to display an overview of the planned study before the planning phase of the study is completed.
    // If "readyToProceed" == "true" enable the "next" button else disable this button.
    case OVERVIEW:
        ui->w_newSturyFirstPage->hide();
        ui->w_SolutionStrategie->hide();
        ui->w_studyProgress->hide();
        ui->w_studyResults->hide();
        ui->w_Overview->showStudyOverview(study);
        ui->w_Overview->show();
        ui->pB_Next->setText("Create study");
        ui->pB_Next->setToolTip("Create a new Study.");
        ui->pB_Back->setEnabled(true);
        ui->actionShow_Details->setEnabled(false);
        ui->actionSave_Study->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
        break;
    // "pageCounter" == STUDY_PROGRESS => display a widget of class "StudyProgressPage"
    // Purpose of this widget is to provide all tools needed to monitor an ongoing study
    case STUDY_PROGRESS:
        ui->w_newSturyFirstPage->hide();
        ui->w_Overview->hide();
        ui->w_SolutionStrategie->hide();
        ui->w_studyResults->hide();
        ui->w_WindowBottom->hide();
        ui->w_studyProgress->setStudy(study);
        ui->actionSave_Study->setEnabled(true);
        ui->actionSave_as->setEnabled(true);
        ui->actionShow_Details->setEnabled(false);
        ui->w_studyProgress->show();
        break;
    // "pageCounter" == STUDY_RESULTS => display a widget of class "StudyEvaluationPage"
    // Purpose of this widget is to display the final results of the study (after all patients are recruited)
    case STUDY_RESULTS:
        ui->w_newSturyFirstPage->hide();
        ui->w_Overview->hide();
        ui->w_SolutionStrategie->hide();
        ui->w_WindowBottom->hide();
        ui->w_studyProgress->hide();
        ui->w_studyResults->setStudy(study);
        ui->w_studyResults->show();
        break;
    }
    this->repaint();
}

//Purpose of this slot is to enable ("readyToProceed" == "true") or disable ("readyToProceed" == "false") the "next" button in the planning phase of a study
void MainWindow::on_ReadyToProceed(bool readyToProceed)
{
    this->readyToProceed = readyToProceed;
    updateView(pageCounter);
}

// Load "Study"-object from file and display the results / current state of the study
void MainWindow::on_OpenStudy()
{    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Study"), "", tr("Study (*.stud);;All Files (*)"));
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
                 QMessageBox::information(this, tr("Unable to open Study"),
                     file.errorString());
                 return;
             }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);
    delete study;
    study = new Study();
    in >> *study;
    file.close();
    this->studyPath = fileName;
    if(study->getFinished())
        pageCounter = STUDY_RESULTS;
    else
        pageCounter = STUDY_PROGRESS;
    updateView(pageCounter);
}

//Close the main window (after the menue "quit" was triggered)
void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

// if "finished" == "true" set the "pageCounter" to "STUDY_RESULTS" and update the view.
void MainWindow::on_studyFinished(bool finished)
{
    if(finished)
    {
        pageCounter = STUDY_RESULTS;
        updateView(pageCounter);
    }
}
