#include "study.h"

Study::Study()
{
    this->isSimondesign = true;
    this->studyName = new QString();
    this->investigator = new QString();
    this->biometrician = new QString();
    this->solutionStrategy = new Result();
    this->solutionStrategy_sbu1 = new Sub1_result();
    this->enrolledPatients = new std::vector<Study::Patient>();
    responses = 0;
    responses_ep2 = 0;
    this->studyStopped = false;
    this->studyFinished = false;
}

Study::~Study()
{
    delete studyName;
    delete investigator;
    delete biometrician;
    delete enrolledPatients;
    delete solutionStrategy_sbu1;
    delete solutionStrategy;
}

void Study::setIsSimondesign(bool isSimon){this->isSimondesign = isSimon;}
bool Study::getIsSimondesign(){return this->isSimondesign;}

void Study::setBiometrician(QString biometrician){ *(this->biometrician) = biometrician;}
QString Study::getBiometrician() const{ return *biometrician;}

void Study::setInvestigator(QString investigator){ *(this->investigator) = investigator;}
QString Study::getInvestigator() const{ return *investigator;}

void Study::setSolutionStrategy(Result* solutionStrategy)
{
    this->isSimondesign = true;
    this->solutionStrategy = solutionStrategy;
}
Result* Study::getSolutionStrategy(){ return solutionStrategy;}

void Study::setSolutionStrategy_Sub1(Sub1_result *solutionStrategy)
{
    this->isSimondesign = false;
    this->solutionStrategy_sbu1 = solutionStrategy;
}
Sub1_result* Study::getSolutionStrategy_Sub1(){return this->solutionStrategy_sbu1;}

void Study::setStudyName(QString studyName){ *(this->studyName) = studyName;}
QString Study::getStudyName() const{ return *studyName;}

std::vector<Study::Patient>* Study::getEnrolledPatients(){ return enrolledPatients;}

bool Study::addPatient(Study::Patient patient)
{
    // Check if a patient with the same id is already enrolled.
    for(unsigned int i = 0; i < enrolledPatients->size(); i++)
    {
        if((enrolledPatients->at(i)).patID == patient.patID)
            return false;
    }

    enrolledPatients->push_back(patient);
    responses = patient.response ? responses +1 : responses;
    if(!(this->isSimondesign))
        responses_ep2 = patient.response_ep2 ? (responses_ep2 + 1) : responses_ep2;
    // Emit patientAdded signal (important for the view/gui).
    emit patientAdded(patient);
    return true;
}

bool Study::removePatient()
{
    if(enrolledPatients->size() > 0)
    {
        Study::Patient p = enrolledPatients->back();
        responses = p.response ? responses - 1 : responses;
        if(!(this->isSimondesign))
            responses_ep2 = p.response_ep2 ? responses_ep2 - 1 : responses_ep2;
        enrolledPatients->pop_back();
        // Emit patientRemoved signal (important for the view/gui).
        emit patientRemoved();
        return true;
    }
    else
        return false;
}

QDataStream & operator <<(QDataStream &dataStream, const Study &study)
{
    dataStream << study.isSimondesign;
    dataStream << *study.studyName;
    dataStream << *study.investigator;
    dataStream << *study.biometrician;
     // Write the number of patients to file (used in the deserialization process).
    int patCount = (int)(study.enrolledPatients->size());
    dataStream << patCount;
    dataStream << study.responses;
    if(!(study.isSimondesign))
        dataStream << study.responses_ep2;
    if(study.isSimondesign)
    {
        dataStream << *study.solutionStrategy;
    }
    else
    {
        dataStream << *study.solutionStrategy_sbu1;
    }

    dataStream << study.alphaNominal;

    std::vector<Study::Patient>::iterator it;
    QFile *file = (QFile*)(dataStream.device());
    for(it = study.enrolledPatients->begin(); it != study.enrolledPatients->end(); it++)
    {
        dataStream << QString((*it).patID);
        bool tmp = (*it).response;
        dataStream << tmp;
        if(!(study.isSimondesign))
        {
            dataStream << (*it).response_ep2;
        }
        file->flush();
    }

    dataStream << study.studyStopped;
    dataStream << study.studyFinished;
    return dataStream;
}

QDataStream & operator >>(QDataStream &dataStream, Study &study)
{
    dataStream >> study.isSimondesign;
    dataStream >> *study.studyName;
    dataStream >> *study.investigator;
    dataStream >> *study.biometrician;
    int patCount;
    dataStream >> patCount;
    dataStream >> study.responses;
    if(!(study.isSimondesign))
        dataStream >> study.responses_ep2;
    if(study.isSimondesign)
    {
        dataStream >> *study.solutionStrategy;
    }
    else
    {
        dataStream >> *study.solutionStrategy_sbu1;
    }

    dataStream >> study.alphaNominal;



    study.enrolledPatients->clear();
    for(int i=0; i < patCount; i++)
    {
        QString patID;
        Study::Patient p;
        dataStream >> patID;
        p.patID = patID;
        bool tmp;
        dataStream >> tmp;
        p.response = tmp;
        if(!(study.isSimondesign))
        {
            dataStream >> p.response_ep2;
        }

        study.enrolledPatients->push_back(p);
    }

    dataStream >> study.studyStopped;
    dataStream >> study.studyFinished;
    return dataStream;
}

int Study::getNumOfResponses(){return responses;}

int Study::getNumOfResponses_ep2()
{
    if(!(this->isSimondesign))
        return responses_ep2;
    else
        return 0;
}

int Study::getNumOfResponses_stageOne()
{
    size_t numPat = enrolledPatients->size();
    int numResponses = 0;
    size_t i;
    Study::Patient tmpPat;

    size_t n1 = (size_t)(this->isSimondesign ? solutionStrategy->n1 : solutionStrategy_sbu1->getN1());


    if(numPat < n1)
    {
        for(i = 0; i < numPat; i++)
        {
            tmpPat = enrolledPatients->at(i);
            numResponses += tmpPat.response ? 1:0;
        }
    }
    else
    {
        for(i = 0; i < n1; i++)
        {
            tmpPat = enrolledPatients->at(i);
            numResponses += tmpPat.response ? 1:0;
        }
    }

    return numResponses;
}

double Study::getCE_flex()
{
    double ce = 0;
    if(this->isSimondesign)
    {
        int n = solutionStrategy->n;

        if((size_t)n <= enrolledPatients->size())
        {
            int k1 = getNumOfResponses_stageOne();

            switch(solutionStrategy->flexSol)
            {
            case Result::Proportional:
                ce = solutionStrategy->flexibleAdaptionProp->find(k1)->second;
                break;
            case Result::Equal:
                ce = solutionStrategy->flexibleAdaptionEqua->find(k1)->second;
                break;
            case Result::ToOne:
                ce = solutionStrategy->flexibleAdaptionToOne->find(k1)->second;
                break;
            case Result::None:
                ce = solutionStrategy->flexibleAdaptionNone->find(k1)->second;
                break;
            default:
                break;
            }

        }
    }

    return ce;
}

bool Study::stopForFutility()
{
    if(this->isSimondesign) // Is a "simon's tow stage design" used?
    {
        if(solutionStrategy->useCurtailment) // Is "stochastic curtailment" used? (Then there are additional stopping rules)
        {
            Result::Curtailment curt = (*solutionStrategy->curtailmentResults->find(solutionStrategy->cut)).second;
            for(unsigned int i=0; i < curt.stoppingRulesNSC->size(); i++)
            {
                // If a stopping rule is violated (less responses observed than needed) return "true".
                if((enrolledPatients->size() == curt.stoppingRulesNSC->at(i)[1]) && (responses <= curt.stoppingRulesNSC->at(i)[0]))
                    return true;
            }
            return false;
        }
        else // "Stochastic curtailment" is not used (only moment to stop for futility is at the interim analysis).
        {
            if((enrolledPatients->size() == solutionStrategy->n1) && (responses <= solutionStrategy->r1) )
                return true;
            return false;
        }
    }
    else // A "subset design" is used.
    {
        if(solutionStrategy_sbu1->getUseCurtailment()) // Is "stochastic curtailment" used? (Then there are additional stopping rules)
        {
            Sub1_result::Curtailment_SubD1 curt = (*solutionStrategy_sbu1->getCurtailmentResults()->find(solutionStrategy_sbu1->getCut())).second;
            Sub1_result::StoppingRule_SubD1 sr;
            for(unsigned int i = 0; i < curt.stoppingRulesNSC->size(); i++)
            {
                sr = curt.stoppingRulesNSC->at(i);
                // If a stopping rule is violated return "true".
                if((enrolledPatients->size() == sr.enrolled_int) && ((responses <= sr.t_int) && (responses_ep2 <= sr.u_int)))
                    return true;
            }
            return false;
        }
        else // "Stochastic curtailment" is not used (only moment to stop for futility is at the interim analysis).
        {
            if((enrolledPatients->size() == solutionStrategy_sbu1->getN1()) && (responses <= solutionStrategy_sbu1->getR1()))
                return true;
            return false;
        }
    }

}

void Study::setAlphaNominal(double alpha){this->alphaNominal = alpha;}

double Study::getAlphaNominal(){return this->alphaNominal;}

void Study::setStopped(bool stop){this->studyStopped = stop;}

bool Study::getStopped(){return this->studyStopped;}

void Study::setFinished(bool finished){this->studyFinished = finished;}

bool Study::getFinished(){return this->studyFinished;}
