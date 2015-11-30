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

#include "result.h"

Result::Result()
{
    this->stoppingRulesNSC = new std::vector<StoppingRule>();
    this->curtailmentResults = new std::map<int, Curtailment>();
    this->flexibleAdaptionProp = new std::map<int, double>();
    this->flexibleAdaptionEqua = new std::map<int, double>();
    this->flexibleAdaptionToOne = new std::map<int, double>();
    this->flexibleAdaptionNone = new std::map<int, double>();
    this->name = new QString();    
}

Result::Result(int n, int r, int n1, int r1, double alpha, double beta, double petP1, double petP0, double enP1, double enP0, int iD, double p0, double p1)
{
    this->n = n;
    this->r = r;
    this->n1 = n1;
    this->r1 = r1;
    this->alpha = alpha;
    this->beta = beta;
    this->petP0 = petP0;
    this->petP1 = petP1;
    this->enP0 = enP0;
    this->enP1 = enP1;
    this->admissible = false;
    this->stoppingRulesNSC = new std::vector<StoppingRule>();
    this->curtailmentResults = new std::map<int, Curtailment>();
    this->flexibleAdaptionProp = new std::map<int, double>();
    this->flexibleAdaptionEqua = new std::map<int, double>();
    this->flexibleAdaptionToOne = new std::map<int, double>();
    this->flexibleAdaptionNone = new std::map<int, double>();
    this->iD = iD;
    this->name = new QString();
    this->p0 = p0;
    this->p1 = p1;
}

Result::~Result()
{
    delete stoppingRulesNSC;
    delete curtailmentResults;
    delete flexibleAdaptionProp;
    delete flexibleAdaptionEqua;
    delete flexibleAdaptionToOne;
    delete flexibleAdaptionNone;
    delete name;
}

void Result::loadResultFromR(QString solObjectName, int solNumber, bool loadCurtailment)
{
    RInside &R = RInside::instance();
    QString solCode;
    QTextStream ts(&solCode, QIODevice::ReadWrite);
    ts << solObjectName;
    ts << "[[1]][" << solNumber << ",]";
    this->n =R.parseEval(solCode.toStdString() + "$n");
    this->r = R.parseEval(solCode.toStdString() + "$r");
    this->n1 = R.parseEval(solCode.toStdString() + "$n1");
    this->r1 = R.parseEval(solCode.toStdString() + "$r1");
    this->alpha = R.parseEval(solCode.toStdString() + "$Alpha");
    this->beta = R.parseEval(solCode.toStdString() + "$Beta");
    this->petP0 = R.parseEval(solCode.toStdString() + "$petP0");
    this->petP1 = R.parseEval(solCode.toStdString() + "$petP1");
    this->enP0 = R.parseEval(solCode.toStdString() + "$enP0");
    this->enP1 = R.parseEval(solCode.toStdString() + "$enP1");
    this->admissible = R.parseEval(solCode.toStdString() + "$Admissible");
    if(this->admissible)
    {
        this->admissibleStart = R.parseEval(solCode.toStdString() + "$Admiss_Start");
        this->admissibleStop = R.parseEval(solCode.toStdString() + "$Admiss_End");

        R.parseEvalQ("tmp <- as.character(" +solCode.toStdString() + "$Type)");
        std::string test = R["tmp"];
        this->name = new QString(QString::fromStdString(test));
    }
    else
    {
        this->admissibleStart = 0;
        this->admissibleStop = 0;

        this->name = new QString();
    }
    this->iD = R.parseEval(solCode.toStdString() + "$ID");    
    this->p0 = R.parseEval(solCode.toStdString() + "$p0");
    this->p1 = R.parseEval(solCode.toStdString() + "$p1");

    // load the curtailment results related to the loaded data
    if(loadCurtailment)
    {
        loadCurResults(solObjectName);
    }
}

void Result::loadCurResults(QString solObjectName)
{
    RInside &R = RInside::instance();
    // 1. Check if there are calculated influences of (non-)stochastic curtailment.
    // 2. Get the number of differnent calculated "curtailmentresults" (influence of (non-)stochastic curtailment with differnt "cut points")
    // 3. Load "curtailmentresults" from R to c++
    QString curCode;
    QString lineNumber;
    QTextStream curTs(&curCode, QIODevice::ReadWrite);
    QTextStream lnTs(&lineNumber, QIODevice::ReadWrite);

    curTs << "\"CurResultsForID:" << this->iD << "\" %in% names(" << solObjectName << "$Curtailment_Results)";
    bool curResultPresent = R.parseEval(curCode.toStdString());
    curCode.clear();

    if(curResultPresent)
    {
        curTs << "nrow(";
            curTs << solObjectName;
            curTs << "[[2]][[" << "'CurResultsForID:" << this->iD << "']]";
        curTs << ")";
        int numOfCurresults = R.parseEval(curCode.toStdString());
        curCode.clear();
        curTs << solObjectName << "[[2]] [[" << "'CurResultsForID:" << this->iD << "']]";
        for(int i = 1; i <= numOfCurresults; i++ )
        {
            Result::Curtailment curResult;
            lnTs << "[" << i << ",]";
            curResult.cut = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Cut");
            curResult.en_sc = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$En_SC");
            curResult.pet_sc = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Pet_SC");
            curResult.type1_errorRate = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Type_1_Errorrate");
            curResult.type2_errorRate = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Type_2_Errorrate");
            curResult.en_lower = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$En_lower");
            curResult.en_upper = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$En_upper");
            curResult.pet_lower = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Pet_lower");
            curResult.pet_upper = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Pet_upper");
            curResult.alpha_lower = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$alpha_lower");
            curResult.alpha_upper = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$alpha_upper");
            curResult.beta_lower = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$beta_lower");
            curResult.beta_upper = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$beta_upper");
            curResult.stoppingRulesNSC = loadStoppingRules(solObjectName, i);
            curtailmentResults->insert(std::pair<int, Curtailment>((int)(curResult.cut*100 +0.5), curResult));
            lineNumber.clear();

        }
    }
}

std::vector<float*>* Result::loadStoppingRules(QString solObjectName, int row)
{
    RInside &R = RInside::instance();
    QString sRulesCode;
    QTextStream ts(&sRulesCode, QIODevice::ReadWrite);
    std::vector<float*> *ri_k_cp = new std::vector<float*>();

    // Check if there are stopping rules present in R
    ts << "\"Stoppingrules_for_Row:" << row << "\" %in% names(" << solObjectName << "$Curtailment_Results$'StoppingrulesForID:" << this->iD << "')";
    bool stoppingRulesPresent = R.parseEval(sRulesCode.toStdString());
    sRulesCode.clear();

    // Load stopping rules from R to c++.
    if(stoppingRulesPresent)
    {
        ts << solObjectName << "$Curtailment_Results$'StoppingrulesForID:" << this->iD << "'$'Stoppingrules_for_Row:" << row << "'";
        Rcpp::DataFrame stoppingRulesR = R.parseEval(sRulesCode.toStdString());
        Rcpp::IntegerVector responses = stoppingRulesR["Needed_responses"];
        Rcpp::IntegerVector observed_patients = stoppingRulesR["Observed_patients"];
        size_t i;
        size_t numRules;
        numRules = responses.length();

        for(i = 0; i < numRules; i++)
        {
            float *entry = new float[3];
            qDebug() << responses[i];
            entry[0] = responses[i];
            qDebug() << observed_patients[i];
            entry[1] = observed_patients[i];
            entry[2] = 0;
            ri_k_cp->push_back(entry);
        }
        return ri_k_cp;
    }
    return ri_k_cp;
}

void Result::setAdmissible(double start, double stop)
{
    this->admissibleStart = start;
    this->admissibleStop = stop;
    this->admissible = true;
}

void Result::setAdmissible(double start, double stop, QString name)
{
    setAdmissible(start, stop);
    *(this->name) = name;
}

void Result::addCurtailmentResult(Curtailment curResult)
{
    // Based on the fact that floating point numbers are not suited as an key for the "curtailmentResults"-map the "cut" is stored in percent.
    curtailmentResults->insert(std::pair<int, Curtailment>((int)(curResult.cut*100 +0.5), curResult));
}

void Result::setUseCurtailment(bool useCurtailment){this->useCurtailment = useCurtailment;}

void Result::setCut(int cut){ this->cut = cut;}

QDataStream & operator <<(QDataStream &dataStream, const Result &result)
{
    dataStream << result.iD;
    dataStream << *(result.name);
    dataStream << result.r1;
    dataStream << result.n1;
    dataStream << result.r;
    dataStream << result.n;
    dataStream << result.p0;
    dataStream << result.p1;
    dataStream << result.enP0;
    dataStream << result.enP1;
    dataStream << result.petP0;
    dataStream << result.petP1;
    dataStream << result.alpha;
    dataStream << result.beta;
    dataStream << result.admissible;
    dataStream << result.admissibleStart;
    dataStream << result.admissibleStop;

    dataStream << result.useCurtailment;
    if(result.useCurtailment)
    {
        dataStream << result.cut;

        // Write the number of curtailment results to file (used in the deserialization process).
        dataStream << result.curtailmentResults->size();
        std::map<int, Result::Curtailment>::iterator it;
        for(it = result.curtailmentResults->begin(); it != result.curtailmentResults->end(); it++)
        {
            Result::Curtailment tmp = (Result::Curtailment)((*it).second);
            dataStream << tmp.alpha_lower;
            dataStream << tmp.alpha_upper;
            dataStream << tmp.beta_lower;
            dataStream << tmp.beta_upper;
            dataStream << tmp.cut;
            dataStream << tmp.en_lower;
            dataStream << tmp.en_sc;
            dataStream << tmp.en_upper;
            dataStream << tmp.pet_lower;
            dataStream << tmp.pet_sc;
            dataStream << tmp.pet_upper;
            dataStream << tmp.type1_errorRate;
            dataStream << tmp.type2_errorRate;

            // Write the number of stopping rules to file (used in the deserialization process).
            dataStream << tmp.stoppingRulesNSC->size();
            for(unsigned int i = 0; i < tmp.stoppingRulesNSC->size(); i++)
            {
                dataStream << tmp.stoppingRulesNSC->at(i)[0];
                dataStream << tmp.stoppingRulesNSC->at(i)[1];
                dataStream << tmp.stoppingRulesNSC->at(i)[2];
            }
        }
    }

    std::map<int, double>::iterator it_prop;
    std::map<int, double>::iterator it_equa;
    std::map<int, double>::iterator it_toOne;
    std::map<int, double>::iterator it_none;

    dataStream << result.useFlexible;
    if(result.useFlexible)
    {
        int tmpFlexSol = result.flexSol;
        dataStream << tmpFlexSol;

        // Needed for deserialization
        size_t flexSize = result.flexibleAdaptionEqua->size();
        dataStream << flexSize;

        for(it_prop = result.flexibleAdaptionProp->begin(); it_prop != result.flexibleAdaptionProp->end(); it_prop++)
        {
            dataStream << it_prop->first;
            dataStream << it_prop->second;
        }
        for(it_equa = result.flexibleAdaptionEqua->begin(); it_equa != result.flexibleAdaptionEqua->end(); it_equa++)
        {
            dataStream << it_equa->first;
            dataStream << it_equa->second;
        }
        for(it_toOne = result.flexibleAdaptionToOne->begin(); it_toOne != result.flexibleAdaptionToOne->end(); it_toOne++)
        {
            dataStream << it_toOne->first;
            dataStream << it_toOne->second;
        }
        for(it_none = result.flexibleAdaptionNone->begin(); it_none != result.flexibleAdaptionNone->end(); it_none++)
        {
            dataStream << it_none->first;
            dataStream << it_none->second;
        }

        QFile *tmp = (QFile*)(dataStream.device());
        tmp->flush();
    }

    return dataStream;
}

QDataStream & operator >>(QDataStream &dataStream, Result &result)
{
    dataStream >> result.iD;
    result.name = new QString();
    dataStream >> *(result.name);
    dataStream >> result.r1;
    dataStream >> result.n1;
    dataStream >> result.r;
    dataStream >> result.n;
    dataStream >> result.p0;
    dataStream >> result.p1;
    dataStream >> result.enP0;
    dataStream >> result.enP1;
    dataStream >> result.petP0;
    dataStream >> result.petP1;
    dataStream >> result.alpha;
    dataStream >> result.beta;
    dataStream >> result.admissible;
    dataStream >> result.admissibleStart;
    dataStream >> result.admissibleStop;

    dataStream >> result.useCurtailment;
    if(result.useCurtailment)
    {
        dataStream >> result.cut;

        size_t curtailCount;
        dataStream >> curtailCount;

        for(size_t i=0; i < curtailCount; i++)
        {
            Result::Curtailment curt;
            dataStream >> curt.alpha_lower;
            dataStream >> curt.alpha_upper;
            dataStream >> curt.beta_lower;
            dataStream >> curt.beta_upper;
            dataStream >> curt.cut;
            dataStream >> curt.en_lower;
            dataStream >> curt.en_sc;
            dataStream >> curt.en_upper;
            dataStream >> curt.pet_lower;
            dataStream >> curt.pet_sc;
            dataStream >> curt.pet_upper;
            dataStream >> curt.type1_errorRate;
            dataStream >> curt.type2_errorRate;

            size_t stoppingRulesCount;
            dataStream >> stoppingRulesCount;
            curt.stoppingRulesNSC = new std::vector<float*>();
            for(unsigned int i = 0; i < stoppingRulesCount; i++)
            {
                float *entry = new float[3];
                dataStream >> entry[0];
                dataStream >> entry[1];
                dataStream >> entry[2];
                curt.stoppingRulesNSC->push_back(entry);
            }

            result.addCurtailmentResult(curt);
        }
    }


    dataStream >> result.useFlexible;
    if(result.useFlexible)
    {
        size_t flexSize;
        size_t i;
        int tmp_k;
        double tmp_ce;
        int tmpFlexSol;
        dataStream >> tmpFlexSol;
        dataStream >> flexSize;        
        result.flexSol = Result::FlexibleSol(tmpFlexSol);
        for(i= 0; i < flexSize; i++)
        {
            dataStream >> tmp_k;
            dataStream >> tmp_ce;
            result.flexibleAdaptionProp->insert(std::pair<int, double>(tmp_k,tmp_ce));
        }
        for(i= 0; i < flexSize; i++)
        {
            dataStream >> tmp_k;
            dataStream >> tmp_ce;
            result.flexibleAdaptionEqua->insert(std::pair<int, double>(tmp_k,tmp_ce));
        }
        for(i= 0; i < flexSize; i++)
        {
            dataStream >> tmp_k;
            dataStream >> tmp_ce;
            result.flexibleAdaptionToOne->insert(std::pair<int, double>(tmp_k,tmp_ce));
        }
        for(i= 0; i < flexSize; i++)
        {
            dataStream >> tmp_k;
            dataStream >> tmp_ce;
            result.flexibleAdaptionNone->insert(std::pair<int, double>(tmp_k,tmp_ce));
        }
    }

    return dataStream;
}

