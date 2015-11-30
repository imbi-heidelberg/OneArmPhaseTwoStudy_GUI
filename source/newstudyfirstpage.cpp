// OneArmPhaseTwoStudy is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// OneArmPhaseTwoStudy is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details (http://www.gnu.org/licenses/).

#include "newstudyfirstpage.h"
#include "ui_newstudyfirstpage.h"

NewStudyFirstPage::NewStudyFirstPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewStudyFirstPage)
{
    ui->setupUi(this);

    ui->le_studyName->setToolTip("Name of the study.");
    ui->le_investigator->setToolTip("Name of the primary investigator.");
    ui->le_biometrician->setToolTip("Name of the involved biometrican.");


    QObject::connect(ui->le_studyName, SIGNAL(textChanged(const QString)), this, SLOT(on_le_studyName_textChanged(const QString)));
    QObject::connect(ui->le_investigator, SIGNAL(textChanged(const QString)), this, SLOT(on_le_investigator_textChanged(const QString)));
    QObject::connect(ui->le_biometrician, SIGNAL(textChanged(const QString)), this, SLOT(on_le_biometrician_textChanged(const QString)));
}

NewStudyFirstPage::~NewStudyFirstPage()
{
    delete ui;
}


bool NewStudyFirstPage::allEntriesValid()
{
    //returns true if all edit-fields are filled out.
    return !(ui->le_studyName->text().isEmpty()) && !(ui->le_investigator->text().isEmpty()) && !(ui->le_biometrician->text().isEmpty());
}


QString NewStudyFirstPage::getStudyName()
{
    return ui->le_studyName->text();
}

QString NewStudyFirstPage::getInvestigator()
{
    return ui->le_investigator->text();
}

QString NewStudyFirstPage::getBiometrician()
{
    return ui->le_biometrician->text();
}

// Removes the text from all "QLineEdit" objects.
void NewStudyFirstPage::reset()
{
    ui->le_studyName->setText("");
    ui->le_investigator->setText("");
    ui->le_biometrician->setText("");
}

void NewStudyFirstPage::on_le_studyName_textChanged(const QString &arg1)
{
    emit readyToProceed(allEntriesValid());
}

void NewStudyFirstPage::on_le_investigator_textChanged(const QString &arg1)
{
    emit readyToProceed(allEntriesValid());
}

void NewStudyFirstPage::on_le_biometrician_textChanged(const QString &arg1)
{
    emit readyToProceed(allEntriesValid());
}
