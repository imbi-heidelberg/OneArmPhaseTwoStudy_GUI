// An object of this data class stores all informations/parameters which belong to one
// simon's two stage design (and optional the informations/parameters regarding stochastic
// curtailment and/or the flexible extension). Additional this class implements operators
// for serialisation and deserialisation of objects from this class.
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


#ifndef RESULT_H
#define RESULT_H

#include <qlistwidget.h>
#include <QTableWidget>
#include <QString>
#include <QTextStream>
#include <map>
#include <vector>
#include <RInside.h>
#include <QDebug>
#include <QFile>

class Result
{
public:
    // A stopping rule determines that the study has to be stopped for futility
    // if there are less than "ri"+1 observed responses among "n" enrolled patients.
    struct StoppingRule
    {
        int ri; // Observed responses.
        int n; // Enrolled patients.
        double pet; // Propability of early termination of the study.
    };

    // Enum um fest zu legen welche rest-alpha-spending strategie gewaehlt wurde
    // falls die flexible-erweiterung benutzt wird.
    enum FlexibleSol {
        Proportional,
        Equal,
        ToOne,
        None
    };
    // Stores the estimated effect of (non-)stochastic curtailment for the design stored in the "Result"-class.
    // Sets the "cut point" used to calculate the effect of (non-)stochastic curtailment. A study is stopped if the conditional power falls below the value of "cut".
    struct Curtailment
    {
        // Determines the "cut point" used to calculate the effect of (non-)stochastic curtailment.
        // A study is stopped if the conditional power falls below the value of "cut".
        float cut;
        float en_sc; // Expected sample size.
        float pet_sc; // Propability for early termination.
        float type1_errorRate;
        float type2_errorRate;
        // Variables for the 95%-confidence intervall
        float en_lower;
        float en_upper;
        float pet_lower;
        float pet_upper;
        float alpha_lower;
        float alpha_upper;
        float beta_lower;
        float beta_upper;
        // Vector for the stopping rules resulting from the "cut point".
        std::vector<float*> *stoppingRulesNSC;
    };

    // Operator for deserialisation
    friend QDataStream &operator<<(QDataStream &dataStream, const Result& result);
    // Operatror for serialisation
    friend QDataStream &operator>>(QDataStream &dataStream, Result& result);

    Result();
    Result(int n, int r, int n1, int r1, double alpha, double beta, double petP1, double petP0, double enP1, double enP0, int iD, double p0, double p1);   
    ~Result();
    // Sets the member variable "admissible" to "true" and stores the range ["start","stop"] for
    // which the design is admissible.
    void setAdmissible(double start, double stop);
    // Sets the member variable "admissible" to "true" and store the range ["start","stop"] for
    // which the design is admissible. In addition a name for the design is stored (like "optimal" or "minimax").
    void setAdmissible(double start, double stop, QString name);
    void addCurtailmentResult(Curtailment curResult);
    // Loads all design parameters from R to c++.
    // solObjectName = Name of the object in R which stores the design parameters (of the differnt calculated designs [at least one]).
    // solNumber = Determines which design should be loaded among the calculated (row number in the corresponding data.frame).
    // loadCurtailment = If set to "true" the method also tries to load all calculated informations regarding (non-)stochastic curtailment
    //                   (for the design to be loaded).
    void loadResultFromR(QString solObjectName,int solNumber, bool loadCurtailment);
    // Stores the estimated effect of (non-)stochastic curtailment. (Adds one element to "curtailmentResults".)
    void loadCurResults(QString solObjectName);
    // Tries to load stopping rules from R to c++ (used if curtailment is used).
    std::vector<float*>* loadStoppingRules(QString solObjectName, int row);
    // Determines whether  or not curtailment is used.
    void setUseCurtailment(bool useCurtailment);
    // Set the "cut point" used for (non-)stochastic curtailment.
    void setCut(int cut);

    // Public members for easy access.
    int iD;
    int n, r, n1, r1;
    double alpha, beta, petP1, petP0, enP1, enP0, admissibleStart, admissibleStop, p0, p1;
    bool admissible;
    QString *name;
    std::vector<StoppingRule> *stoppingRulesNSC;
    std::map<int, Curtailment> *curtailmentResults;
    std::map<int, double> *flexibleAdaptionProp;
    std::map<int, double> *flexibleAdaptionEqua;
    std::map<int, double> *flexibleAdaptionToOne;
    std::map<int, double> *flexibleAdaptionNone;

    bool useFlexible;
    FlexibleSol flexSol;
    bool useCurtailment;
    int cut;
};

#endif // RESULT_H
