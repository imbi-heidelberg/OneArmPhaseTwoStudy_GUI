// An object of this class represents the first page / form
// which has to be filled out in the planning phase of a study.
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


#ifndef NEWSTUDYFIRSTPAGE_H
#define NEWSTUDYFIRSTPAGE_H

#include <QWidget>

namespace Ui {
    class NewStudyFirstPage;
}

class NewStudyFirstPage : public QWidget
{
    Q_OBJECT

public:
    // Default constructor
    explicit NewStudyFirstPage(QWidget *parent = 0);
    // Default desturctor
    ~NewStudyFirstPage();
    // Returns the entered study name
    QString getStudyName();
    // Returns the entered investigator
    QString getInvestigator();
    // Returns the entered biometrician
    QString getBiometrician();
    void reset();

signals:
    // Signal to inform the "MainWindow"-object whether or not the "next"-button
    // should be enabled. (If "ready" == "true" enable "next"-button else disable the button)
    void readyToProceed(bool ready);

private:
    Ui::NewStudyFirstPage *ui;
    // Returns "ture" if every text field ("QLineEdit" object) is filled out.
    // Returns "false" otherwise
    bool allEntriesValid();

private slots:
    // Purpose of the three slots below is to ensure that the "next"-button can only be pressed if all
    // three "QLineEdit" objects have text entered by the user.

    // Catches every text change in "le_studyName" and emits the "readyToProceed" signal.
    void on_le_studyName_textChanged(const QString &arg1);
    // Catches every text change in "le_investigator" and emits the "readyToProceed" signal.
    void on_le_investigator_textChanged(const QString &arg1);
    // Catches every text change in "le_biometrician" and emits the "readyToProceed" signal.
    void on_le_biometrician_textChanged(const QString &arg1);
};

#endif // NEWSTUDYFIRSTPAGE_H
