// An object of this data class stores all informations/parameters which belong to one
// subset design (and optional the informations/parameters regarding stochastic
// curtailment). Additional this class implements operators
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

#ifndef SUB1_RESULT_H
#define SUB1_RESULT_H

#include <QString>
#include <QTextStream>
#include <map>
#include <vector>
#include <RInside.h>
#include <QDataStream>
#include <QDebug>


class Sub1_result
{
public:

    struct StoppingRule_SubD1
    {
    public:
        // A stopping rule determines that the study has to be stopped for futility
        // if there are less than "t_int"+1 responses for the subset endpoint and
        // less than "u_int"+1 responses for the superset endpoint are observed among "enrolled_int" enrolled patients.
        StoppingRule_SubD1() : t_int(0), u_int(0), enrolled_int(0), cp(0) {}
        //StoppingRule_SubD1(int t, int u, int enrolled, long double c_p) : t_int(t), u_int(u), enrolled_int(enrolled), cp(c_p) {}
        int t_int, u_int, enrolled_int;
        long double cp;
    };

    // Stores the estimated effect of (non-)stochastic curtailment for the design stored in the "Sub1_result"-class.
    struct Curtailment_SubD1
    {
    public:
        // Determines the "cut point" used to calculate the effect of (non-)stochastic curtailment.
        // A study is stopped if the conditional power falls below the value of "cut".
        float cut;
        float en_sc; // Expected sample size
        float pet_sc; // Propability for early termination.
        float type1_errorRate;
        float type2_errorRate;
        // Vector for the stopping rules resulting from the "cut point".
        std::vector<StoppingRule_SubD1> *stoppingRulesNSC;
    };

    Sub1_result();
    Sub1_result(int n, int r, int s, int n1, int r1, double alpha, double beta, double petP0, double enP0, int iD, double pc0, double pt0, double pc1, double pt1);
    ~Sub1_result();

    // Definition of getters and setters for the member variables.
    bool getAdmissible() const;
    // Sets the member variable "admissible" to "true" and stores the range ["start","stop"] for
    // which the design is admissible.
    void setAdmissible(double start, double stop);
    // Sets the member variable "admissible" to "true" and store the range ["start","stop"] for
    // which the design is admissible. In addition a name for the design is stored (like "optimal" or "minimax").
    void setAdmissible(double start, double stop, QString name);
    double getAdmissibleStart() const;
    double getAdmissibleStop() const;
    void setAlpha(double alpha);
    double getAlpha() const;  // Get type one error rate.
    void setBeta(double beta);
    double getBeta() const; // Get type two error rate.
    int getN() const; // Get overall sample size.
    void setN(int n);
    int getR() const; // Get the overall critical value for the subset endpoint (more than "r" responses under "n" patients needed to reject the null hypothesis).
    void setR(int r);
    int getS()  const; // Get the overall critical value for the superset endpoint.
    void setS(int s);
    int getN1()  const; // Get sample size for the first stage of the design.
    void setN1(int n1);
    int getR1()  const; // Get the critical value for the first stage of the study.
    void setR1(int r1);
    double getEnP0()  const; // Get expected sample size under the null hypothesis.
    void setEnP0(double enP0);
    double getPetP0()  const; // Get probability of early termination under the null hypothesis.
    void setPetP0(double petP0);
    // Set the probability of an response for the subset endpoint under the null hypothesis.
    void setPc0(double pc0);
    double getPc0() const;
    // Set the probability of an response for the superset endpoint under the null hypothesis.
    void setPt0(double pt0);
    double getPt0() const;
    // Set the probability of an response for the subset endpoint under the alternative hypothesis.
    void setPc1(double pc1);
    double getPc1() const;
    // Set the probability of an response for the superset endpoint under the alternative hypothesis.
    void setPt1(double pt1);
    double getPt1() const;

    // Determines whether  or not curtailment is used.
    void setUseCurtailment(bool useCurtailment);
    bool getUseCurtailment() const;
    void setCut(int cut);  // Set the "cut point" used for (non-)stochastic curtailment.
    int getCut();

    int getID() const;
    void setID(int iD);

    void addCurtailmentResult(Curtailment_SubD1 curResult);
    std::map<int, Curtailment_SubD1>* getCurtailmentResults();

    QString* getName() const;
    void setName(QString* name);

    // Loads all design parameters from R to c++.
    // solObjectName = Name of the object in R which stores the design parameters (of the differnt calculated designs [at least one]).
    // solNumber = Determines which design should be loaded among the calculated (row number in the corresponding data.frame).
    // loadCurtailment = If set to "true" the method also tries to load all calculated informations regarding (non-)stochastic curtailment
    //                   (for the design to be loaded).
    void loadResultFromR(QString solObjectName,int solNumber, bool loadCurtailment);
    // Stores the estimated effect of (non-)stochastic curtailment. (Adds one element to "curtailmentResults".)
    // solObjectName = Name of the object in R which stores the design parameters (of the differnt calculated designs [at least one]).
    void loadCurResults(QString solObjectName);
    // Tries to load stopping rules from R to c++ (used if curtailment is used).
    std::vector<StoppingRule_SubD1>* loadStoppingRules(QString solObjectName, int row);

    // Operator for deserialisation
    friend QDataStream &operator<<(QDataStream &dataStream, const Sub1_result& result);
    // Operatror for serialisation
    friend QDataStream &operator>>(QDataStream &dataStream, Sub1_result& result);

    std::map<int, Curtailment_SubD1> *curtailmentResults;

private:
    int iD;
    int n, r, n1, r1, s;
    double alpha, beta, petP0, enP0, admissibleStart, admissibleStop;
    double pc0, pt0, pc1, pt1;
    bool admissible;
    bool useCurtailment;
    // If stochastic curtailment is used "cut" stores the chosen cutpoint in percent.
    int cut;
    QString *name;


};

#endif // SUB1_RESULT_H
