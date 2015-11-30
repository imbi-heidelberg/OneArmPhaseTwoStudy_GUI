// An object of this class implements all methods needed for planning, monitoring,
// and analyzing subset designs.
// (Internally the appropriated methods from the "OneArmePhaseTwoStudy" r-package are used.)
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

#ifndef R_SUB1DESIGN_H
#define R_SUB1DESIGN_H

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "OneArmPhaseTwoStudy"
#endif
#define SOLNAME_SUB1 "sub1Sol"
#define SUB1_OBJECT "sub1"

#include <RInside.h>
#include <QString>
#include <QTextStream>
#include <QtGui>
#include "sub1_result.h"
#include <QDebug>

class R_Sub1Design
{
public:
    R_Sub1Design();
    R_Sub1Design(long double alpha, long double beta, long double pc0, long double pt0, long double pc1, long double pt1);

    ~R_Sub1Design();
    // Tries to load the "OneArmPhaseTwo"-package. If successful the method returns "true" otherwise "false".
    bool loadPackage();
    // Identifies simon's tow stage designs which fit to the current values of the member variables "alpha", "beta", "pc0",
    // "pt0", "pc1", "pt1" and "maxn".
    // skipS: If true skips the iteration over "s" at certian points to improve calculation speed (finds less designs).
    // skipR: If true skips the iteration over "r" at certian points to improve calculation speed (finds less designs).
    // skipN1: If true skips the iteration over "n1" at certian points to improve calculation speed
    //         (finds less designs and it is impossible to determine the optimalization criteria of the found designs).
    // lowerBoader: Sets a minimal value for "n" (number of patients to be recruited).
    // upperBoader: Sets a maximal value for "n" (number of patients to be recruited).
    void calculateStudySolutions(bool skipS, bool skipR, bool skipN1, int lowerBoader, int upperBoader);
    // Returns the number of identified designs (after "calculateStudySolutions" was invoked).
    int getNumberOfResults();
    // Estimates the effect of (non-)stochastic curtailment for the "subset" design with the ID "resID".
    // cut: Sets the "cut point" used to calculate the effect of (non-)stochastic curtailment. A study is stopped if the conditional power falls below the value of "cut".
    // reps: Number of replications used to estimate the effect of (non-)stochastic curtailment.
    // all: If true the effect of (non-)stochastic curtailment will be calculated for different cut points in 0.05 steps starting with the value of the parameter "cut".
    void calculateSC(int resID, double cut, int reps, bool all);
    // Calculates the "uniformly minimal variance unbiased estimator" (UMVUE)
    // for the true response rate for the subset endpoint based on the approach of Grishick, Mosteller and Savage.
    // t: Observed responses in the subset endpoint.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // n: Overall sample size.
    double get_UMVUE_ep1(int t, int r1, int n1, int n);
    // Calculates the "uniformly minimal variance unbiased estimator" (UMVUE) for the true response rate for the superset endpoint.
    // t: Observed responses in the subset endpoint.
    // u: Observed responses in the superset endpoint.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // n: Overall sample size.
    double get_UMVUE_ep2(int t, int u, int r1, int n1, int n);
    // Returns the conditional power when "t" responses in the supset and "u" responses in the superset endpoint
    // where observed out of "enrolled" Patients for the given Simon's tow stage design.
    // t: Observed responses in the subset endpoint.
    // u: Observed responses in the superset endpoint.
    // enrolled: Number of enrolled patients.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // r: Critical value for the subset endpoint.
    // s: Critical value for the superset endpoint.
    // n: Overall sample size.
    // pc1: The response probability under the alternative hypothesis for the subset endpoint.
    // pt1: The response probability under the alternative hypothesis for the superset endpoint.
    double get_conditionalPower(int t, int u, int enrolled, int r1, int n1, int r, int s, int n, double pc1, double pt1);
    // Calculates the exact p value for a given subset design.
    // t: Observed responses in the subset endpoint.
    // u: Observed responses in the superset endpoint.
    // enrolled: Number of enrolled patients.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // n: Overall sample size.
    // pc0: The response probability for the subset endpoint under the null hypothesis.
    // pt0: The response probability for the superset endpoint under the null hypothesis.
    double get_pValue(int t, int u , int r1, int n1, int n, double pc0, double pt0);

    // Approximate the maximal value of "n" for the previously set values of
    // "alpha", "beta", "pc0", "pt0", "pc1" and "pt1" in a way that the optimal design should be found through "calculateStudySolutions".
    int aproximateMaxN();

    // Definition of getters and setters for the member variables.
    void setAlpha(double alpha);
    double getAlpha();
    void setBeta(double beta);
    double getBeta();
    void setPc0(double pc0);
    double getPc0();
    void setPt0(double pt0);
    double getPt0();
    void setPc1(double pc1);
    double getPc1();
    void setPt1(double pt1);
    double getPt1();
    // Returns the ID of the minimax design under the identified designs.
    int getMinimaxPos();
    // Returns the ID of the optimal design under the identified designs.
    int getOptimalPos();


     Sub1_result* getResult(int id);

private:
    RInside &R;
    bool packageLoaded;
    bool solutionsCalculated;
    std::vector<Sub1_result*> *allResults;

    double alpha, beta, pc0, pt0, pc1, pt1;
};

#endif // R_SUB1DESIGN_H
