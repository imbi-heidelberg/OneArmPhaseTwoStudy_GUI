// An object of this data class is used to display graphs produced in R
// by invoking one of the following functions provided by the
// OneArmPhaseTwoStudy-R-package: "plot_confidence_set", "plot_simon_study_state",
// "plot_sub1_study_state".
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
#ifndef RWIDGET_H
#define RWIDGET_H

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "PhaseIIPaket"
#endif

#include <QWidget>
#include <RInside.h>
#include <QImage>
#include <QPainter>
#include "study.h"
#include <QTextStream>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

namespace Ui {
class Rwidget;
}

class Rwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Rwidget(QWidget *parent = 0);
    ~Rwidget();
    // Set a pointer to the "Study"-object which should be graphicaly displayed.
    void setStudy(Study *study);
    // Draw the study state of a study with a simon's tow stage design to an temporary image
    // and display that image.
    void drawStudyProgressToImage();
    // Draw the study state of a study with a simon's tow stage design to an temporary image
    // and display that image.
    void drawStudyProgressToImage(int width, int height);
    // Draw the study state of a study with a subset design to an temporary image
    // and display that image.
    void drawStudyProgressToImage_sub1();
    void drawStudyProgressToImage_sub1(int width, int height);
    // Draw a confidence set to an temporary image
    // and display that image.
    void drawConfidenceSet();
    void drawConfidenceSet(int width, int height);
    // Determines if the enrolled patients should be displayed when invoking "drawStudyProgressToImage" or "drawStudyProgressToImage_sub1"
    void setDisplayPatients(bool displayPat);
    // Determines whether or not the stopping rules are loaded in R.
    void setSimonStoppingRulesSet(bool isSet);


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

public slots:
    // Redraw widged after a new patient was added/removed
    void newPatientAdded(Study::Patient p);
    void patientRemoved();
private slots:
    void repaint();


private:
    // Tries to load the "OneArmPhaseTwo"-package. If successful the method returns "true" otherwise "false".
    bool loadPackage();
    Ui::Rwidget *ui;
    RInside &R;
    QString tmpFilePath;
    QImage *image;
    QImage *sacledImage;
    Study *study;
    bool studySet;
    bool displayPat;
    bool plotStudyState;
    bool sub1_stoppingrules_set;
    bool simon_stoppingrules_set;
    bool packageLoaded;
    // The timer is needed for the resize event (little hacky but prevents random crashes
    // which must be related to some timing issues between R and c++)
    QTimer *redrawTimer;
};

#endif // RWIDGET_H
