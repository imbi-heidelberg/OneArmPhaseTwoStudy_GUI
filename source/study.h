// An object of this data class stores all informations/parameters which belong to one
// planned / performed study. (Including the used study design stored in an "Result"-object
// or "Sub1_result"-object.)
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

#ifndef STUDY_H
#define STUDY_H

#include <QString>
#include "result.h"
#include "sub1_result.h"
#include <map>
#include <QDate>
#include <QObject>
#include <QFile>

class Study : public QObject
{
    Q_OBJECT
public:
    // Struct to store all relevant data from one enrolled patient.
    struct Patient
    {
        QString patID; // Unique identifier for the patient
        bool response; // Response in endpoint one (for "simon's two stage design" and/or the "subset design"
        bool response_ep2; // Response in endpoint two (for "subset designs" only)
    };

    // Default constructor.
    Study();
    // Constroctor for studies with a "simon's two stage design".
    //Study(QString studyName, QString investigator, QString biometrician, Result* solutionStrategy);
    // Constroctor for studies with a "subset design".
    //Study(QString studyName, QString investigator, QString biometrician, Sub1_result *solutionStrategy);
    ~Study();
    // Definition of all needed setters and getters.
    void setStudyName(QString studyName);
    QString getStudyName() const;

    void setInvestigator(QString investigator);
    QString getInvestigator() const;

    void setBiometrician(QString biometrician);
    QString getBiometrician() const;

    void setSolutionStrategy(Result* solutionStrategy);
    Result* getSolutionStrategy() ;

    void setSolutionStrategy_Sub1(Sub1_result* solutionStrategy);
    Sub1_result* getSolutionStrategy_Sub1() ;

    std::vector<Study::Patient>* getEnrolledPatients();

    void setAlphaNominal(double alpha);
    double getAlphaNominal();

    void setIsSimondesign(bool isSimon);
    bool getIsSimondesign();

    int getNumOfResponses();
    int getNumOfResponses_ep2();
    int getNumOfResponses_stageOne();

    // Returns the conditional error for the current study state (observed responses among the enrolled patients)
    double getCE_flex();

    // Sets a flag whether or not the study is stopped. (A reason for stopping could be "stopping for futility".)
    void setStopped(bool stop);
    bool getStopped();

    void setFinished(bool finished);
    bool getFinished();
    // End of setter and getter definition.

    // Adds an patient to the study. If an patient with the same ID is allready enrolled the patient is not added and "false" is returned.
    // Otherwise the function returns "true" and emmits the "patientAdded" signal.
    bool addPatient(Study::Patient patient);
    //Loescht insofern moeglich den letzten Patienten in der liste.
    //Sollte kein patient mehr aufgenommen sein wird "false" zurueckgeliefert. Sonst "true
    // If there are currently enrolled patient stored in the "Study"-object the most recently added patient is removed and the function
    // returns "true". If there are no patient stored the function returns "false".
    bool removePatient();
    // Returns "true" if the study has to be stopped for futility (regarding the current study state / enrolled patients).
    // Otherwise the function returns "false".
    bool stopForFutility();

    // Operator for deserialisation
    friend QDataStream &operator<<(QDataStream &dataStream, const Study& study);
    // Operatror for serialisation
    friend QDataStream &operator>>(QDataStream &dataStream, Study& study);

signals:
    void patientAdded(Study::Patient p);
    void patientRemoved();

private:
    bool isSimondesign; // Determines which design is used in this study ( "ture" => "simon's tow stage design"; "false" => "subset design")
    QString *studyName;
    QString *investigator;
    QString *biometrician;
    Result* solutionStrategy;
    Sub1_result *solutionStrategy_sbu1;
    std::vector<Study::Patient> *enrolledPatients;
    int responses;
    int responses_ep2;
    double alphaNominal;
    bool studyStopped;
    bool studyFinished;
};

#endif // STUDY_H
