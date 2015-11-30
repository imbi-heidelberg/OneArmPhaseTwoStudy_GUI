// An object of this class implements all methods needed for planning, monitoring,
// and analyzing simon's tow stage designs.
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


#ifndef R_SIMONDESIGN_H
#define R_SIMONDESIGN_H
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "OneArmPhaseTwoStudy"
#endif
#define SOLNAME "simonSol"
#define SIMON_OBJECT "s"


#include "result.h"
#include <RInside.h>
#include <QString>
#include <QTextStream>
#include <QtGui>
#include "study.h"
#include <QDebug>

class R_simondesign
{
public:
    R_simondesign();
    R_simondesign(double alpha, double beta, double p0, double p1);
    ~R_simondesign();
    // Tries to load the "OneArmPhaseTwo"-package. If successful the method returns "true" otherwise "false".
    bool loadPackage();
    // Identifies simon's tow stage designs which fit to the current values of the member variables "alpha", "beta", "p0", "p1" and "maxn"
    void calculateStudySolutions();
    // Estimates the effect of (non-)stochastic curtailment for the "simon's tow stage" design with the ID "resID".
    // cut: Sets the "cut point" used to calculate the effect of (non-)stochastic curtailment. A study is stopped if the conditional power falls below the value of "cut".
    // reps: Number of replications used to estimate the effect of (non-)stochastic curtailment.
    // all: If true the effect of (non-)stochastic curtailment will be calculated for different cut points in 0.05 steps starting with the value of the parameter "cut".
    void calculateSC(int resID, double cut, int reps, bool all);
    // Returns the ID of the minimax design under the identified designs.
    int getMiniMaxPos();
    // Returns the ID of the optimal design under the identified designs.
    int getOptimalPos();
    // Calculates the conditional power for a given Simon's two-stage design in the interim analysis
    // if the number of patients which should be enrolled in the second stage is altert to "n2".
    // If something wents wrong (R-package can't be loaded) the method returns "-1".
    // n2: Number of patients to be enrolled in the second stage of the study.
    // res: Pointer to the "Result"-object (which contains the design parameters for which the new conditional power regarding to "n2" is calculated).
    // responses: Number of responses observed at the interim analysis.
    // alpha: Overall significance level the trial was planned for.
    double getCP_flex(int n2, Result *res, int responses, double alpha);
    //Calculates the number of patients which should be enrolled in the second stage of a study with the given design
    // ("res") if the conditional power should be altert to "cp".
    // If something wents wrong (R-package can't be loaded) the method returns "-1".
    // cp: Conditional power to which the number of patients for the second stage should be adjusted.
    // res: Pointer to the "Result"-object (which contains the design parameters for which the new "n2" is calculated).
    // responses: Number of responses observed at the interim analysis.
    // alpha: Overall significance level the trial was planned for.
    int getN(double cp, Result *res, int responses, double alpha);
    //folgende Drei methoden ermoeglichen es den rest zwischen aktuellem alpha-niveau eines bestimmten desigens und dem geforderten (alpha - alphaStrich)
    //auf zu teilen so das dieser nach der zwischenauswertung in der zweiten stufe der Studie genutzt werden kann

    // Implements the usage of the conditional error function and 4 differnet ways of spending the
    // "rest alpha"(difference between nominal alpha level and actual alpha level) as proposed by Englert S, and Kiser M in the article
    // "Improving the flexibility and efficiency of phase II designs for oncology trails", Biometrics, 68(3), 886-892.
    // The four differnt ways to spend the "rest alpha" are:
    // None: Calculates the conditional error for all possible outcomes at the interim analysis (different number of responses) using no "rest alpha" spending.
    // To one: Spending the "rest alpha"  only to increase the worst case (smallest conditional error value that is not equal to 0).
    // Equally: Spending the "rest alpha" equally.
    // Proportional: Spending "rest alpha" proportionally. [Taking into account the propability of of obersation of a given study situation at the intermin analysis
    //               (Number of responses observered at the interim analysis)].
    void calcultateCE(int resID);
    // Calculates the number of responses needed for the second stage of a Simon's two-stage design if the flexible extension is chosen in the planning phase.
    // ce: Conditional error for the second stage.
    // p0: Probability for a response under the null hypothesis.
    // n2: Sample size for the second stage.
    int getR2forFlex(double ce, double p0, int n2);
    // Calculates the p-value (binomial test) for
    // "k" observed responses under
    // "n2" enrolled patients
    // with the response probability "p0" for an event under the null hypothesis.
    double getPvalueSingelStage(int k, double p0, int n2);

    // Sets the maximal type I error rate. (Used for design identification.)
    void setAlpha(double a);
    // Sets the maximal type II error rate. (Used for design identification.)
    void setBeta(double b);
    // Sets the response probability under the null hypothesis.
    void setP0(double p0);
    // Sets the response probability under the alternative hypothesis.
    void setP1(double p1);
    //Legt den Bereich fest in dem nach Loesungen fuer das Studiendesigen gesucht wird ( 0..maxN). Kann auch ueber "aproximateMaxN" aproximiert werden.
    // Sets the overall maximal sample size ("maxn") used for design identification in "calculateStudySolutions".
    void setMaxN(int maxN);

    // Calculates the "uniformly minimal variance unbiased estimator" (UMVUE)
    // for the true response rate based on the approach of Grishick, Mosteller and Savage.
    double get_UMVUE(int k, int r1, int n1, int n);    
    // Calculates the p-value for a Simon's two-stage design based on the work from Koyama and Chen.
    // k: Overall observed responses.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // n: Overall sample size.
    // p0: Response probability under the null hypothesis.
    double get_pValue(int k, int r1, int n1, int n, double p0);
    // Calculates the two sided 1-2*alpha confidence interval based on the work from Koyama and Chen.
    // k: Overall observed responses.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // n: Overall sample size.
    // alpha: Determines the two sided 1-2*alpha confidence interval.
    // percision: Determines the precision (in decimal digits) to which the confidence interval should be calculated (should be less than 10).
    double* get_CI(int k, int r1, int n1, int n, double alpha, int percision);
    // Returns the conditional power when "k" responses where observed out of "numPat" Patients for the given Simon's tow stage design.
    // k: Number of observed responses
    // numPat: Number of enrolled patients.
    // r1: Critical value for the first stage.
    // n1: Sample size for the first stage.
    // r: Critical value for the subset endpoint.
    // n: Overall sample size.
    // p1: Response rate under the alternative hypothesis.
    double getConditionalPower(int k, int numPat, int r1, int n1, int r, int n, double p1);
    // Approximate the maximal value of "n" for the previously set values of
    // "alpha", "beta", "p1" and "p0" in a way that the optimal design should be found through "calculateStudySolutions".
    int aproximateMaxN();
    Result *getResult(int id);
    int getNumberOfResults();
private:
    double alpha, beta, p0, p1;//, cut;
    int miniMaxPos, optimalPos, maxn;
    std::vector<Result*> *allResults;

    RInside &R;
    bool packageLoaded;
    bool solutionsCalculated;
    //erzeugt aus dem uebergebenen Result ein dataframe in R unter dem uebergebenen namen.
    // Transfers the data stored in "res" to R in an data.frame with the varibale-name "name".
    void resultToR(Result *res, QString name);
    void calculate_CE_proportional(Result *res, QString name);
    void calculate_CE_equally(Result *res, QString name);
    void calculate_CE_tolowest(Result *res, QString name);
    void calculate_CE_none(Result *res, QString name);
};

#endif
