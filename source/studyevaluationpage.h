// An object of this class is used to display the final state of a study (after enrollment of "n" patients).
// Also key variables like the UMVUE (uniformly minimum unbiased estimator), CI (confidence intervall) and p-value
// are calculated and displayed. (So that the user can make a decision about rejecting or accepting the null hypothesis.)
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

#ifndef STUDYEVALUATIONPAGE_H
#define STUDYEVALUATIONPAGE_H

#include <QWidget>
#include <study.h>
#include <r_simondesign.h>
#include <r_sub1design.h>

namespace Ui {
class StudyEvaluationPage;
}

class StudyEvaluationPage : public QWidget
{
    Q_OBJECT
    
public:
    explicit StudyEvaluationPage(QWidget *parent = 0);
    ~StudyEvaluationPage();

    // Sets the study which has to be evaluated and displayed.
    void setStudy(Study *study);
    
private:

    // Evaluate and display the given "Study"-object
    void updateView(Study *study);

    Ui::StudyEvaluationPage *ui;
    // The following two members are needed to calculate the CI, UMVUE and p-value
    R_simondesign *simon;
    R_Sub1Design *sub1;

    Study *currentStudy;
};

#endif // STUDYEVALUATIONPAGE_H
