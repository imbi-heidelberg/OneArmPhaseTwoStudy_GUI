// An object of this class represents the third page / form
// which is displayed in the planning phase of a study.
// On this page the final dessissions the user made during
// the planning phase of the study are displayed.
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

#pragma once

#ifndef NEWSTUDYOVERVIEW_H
#define NEWSTUDYOVERVIEW_H

#include <QWidget>
#include "study.h"
#include <QTextStream>

namespace Ui {
    class NewStudyOverviewPage;
}

class NewStudyOverviewPage : public QWidget
{
    Q_OBJECT

public:
    explicit NewStudyOverviewPage(QWidget *parent = 0);
    ~NewStudyOverviewPage();
    // Display all informations stored in the given "Study"-object.
    void showStudyOverview(Study *study);

private:
    Ui::NewStudyOverviewPage *ui;
};

#endif // NEWSTUDYOVERVIEW_H
