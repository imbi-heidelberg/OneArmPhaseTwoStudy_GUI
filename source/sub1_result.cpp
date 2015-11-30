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

#include "sub1_result.h"

Sub1_result::Sub1_result()
{
  admissible = false;
  useCurtailment = false;
  this->name = new QString();
  curtailmentResults = new std::map<int, Curtailment_SubD1>();
}

Sub1_result::Sub1_result(int n, int r, int s, int n1, int r1, double alpha, double beta, double petP0, double enP0, int iD,
                double pc0, double pt0, double pc1, double pt1)
{
  this->n = n;
  this->r = r;
  this->s = s;
  this->n1 = n1;
  this->r1 = r1;
  this->alpha = alpha;
  this->beta = beta;
  this->petP0 = petP0;
  this->enP0 = enP0;
  this->admissible = false;
  this->iD = iD;
  this->pc0 = pc0;
  this->pt0 = pt0;
  this->pc1 = pc1;
  this->pt1 = pt1;
  this->name = new QString();
  this->useCurtailment = false;
  curtailmentResults = new std::map<int, Curtailment_SubD1>();
}

Sub1_result::~Sub1_result()
{
  delete name;
    delete curtailmentResults;
}

bool Sub1_result::getAdmissible() const{ return this->admissible;}


void Sub1_result::setAdmissible(double start, double stop)
{
  this->admissibleStart = start;
  this->admissibleStop = stop;
  this->admissible = true;
}

void Sub1_result::setAdmissible(double start, double stop, QString name)
{
  setAdmissible(start, stop);
  *(this->name) = name;
}

double Sub1_result::getAdmissibleStart() const{ return this->admissibleStart;}

double Sub1_result::getAdmissibleStop() const{ return this->admissibleStop;}

double Sub1_result::getAlpha()const {return this->alpha;}
void Sub1_result::setAlpha(double alpha){this->alpha = alpha;}

double Sub1_result::getBeta() const{return this->beta;}
void Sub1_result::setBeta(double beta){this->beta = beta;}

int Sub1_result::getN() const{return this->n;}
void Sub1_result::setN(int n){this->n = n;}

int Sub1_result::getR() const{return this->r;}
void Sub1_result::setR(int r){this->r = r;}

int Sub1_result::getS() const{return this->s;}
void Sub1_result::setS(int s){this->s = s;}

int Sub1_result::getN1() const{return this->n1;}
void Sub1_result::setN1(int n1){this->n1 = n1;}

int Sub1_result::getR1() const{return this->r1;}
void Sub1_result::setR1(int r1){this->r1 = r1;}

double Sub1_result::getEnP0() const{return this->enP0;}
void Sub1_result::setEnP0(double enP0){this->enP0 = enP0;}

double Sub1_result::getPetP0() const{return petP0;}
void Sub1_result::setPetP0(double petP0){this->petP0 = petP0;}

void Sub1_result::setPc0(double pc0){ this->pc0 = pc0;}

double Sub1_result::getPc0() const{return this->pc0;}

void Sub1_result::setPt0(double pt0){this->pt0 = pt0;}

double Sub1_result::getPt0() const{return this->pt0;}

void Sub1_result::setPc1(double pc1){this->pc1 = pc1;}

double Sub1_result::getPc1() const{return this->pc1;}

void Sub1_result::setPt1(double pt1){this->pt1 =pt1;}

double Sub1_result::getPt1() const{return this->pt1;}

void Sub1_result::addCurtailmentResult(Curtailment_SubD1 curResult)
{
    // Based on the fact that floating point numbers are not suited as an key for the "curtailmentResults"-map the "cut" is stored in percent.
    curtailmentResults->insert(std::pair<int, Curtailment_SubD1>((int)(curResult.cut*100 +0.5), curResult));
    this->useCurtailment = true;
}

std::map<int, Sub1_result::Curtailment_SubD1>* Sub1_result::getCurtailmentResults(){return curtailmentResults;}

void Sub1_result::setUseCurtailment(bool useCurtailment){this->useCurtailment = useCurtailment;}
bool Sub1_result::getUseCurtailment() const{return this->useCurtailment;}

void Sub1_result::setCut(int cut){ this->cut = cut;}

int Sub1_result::getCut(){ return this->cut;}

int Sub1_result::getID() const {return this->iD;}

void Sub1_result::setID(int iD)
{
    this->iD = iD;
}

QString* Sub1_result::getName() const{return this->name;}

void Sub1_result::setName(QString *name)
{
    this->name = name;
}

void Sub1_result::loadResultFromR(QString solObjectName, int solNumber, bool loadCurtailment)
{
    RInside &R = RInside::instance();
    QString solCode;
    QTextStream ts(&solCode, QIODevice::ReadWrite);
    ts << solObjectName;
    ts << "[[1]][" << solNumber << ",]";    
    this->n =R.parseEval(solCode.toStdString() + "$n");
    this->r = R.parseEval(solCode.toStdString() + "$r");
    this->s = R.parseEval(solCode.toStdString() + "$s");
    this->n1 = R.parseEval(solCode.toStdString() + "$n1");
    this->r1 = R.parseEval(solCode.toStdString() + "$r1");
    this->alpha = R.parseEval(solCode.toStdString() + "$Alpha");
    this->beta = R.parseEval(solCode.toStdString() + "$Beta");
    this->petP0 = R.parseEval(solCode.toStdString() + "$petP0");;
    this->enP0 = R.parseEval(solCode.toStdString() + "$enP0");
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
    this->pc0 = R.parseEval(solCode.toStdString() + "$pc0");
    this->pt0 = R.parseEval(solCode.toStdString() + "$pt0");
    this->pc1 = R.parseEval(solCode.toStdString() + "$pc1");
    this->pt1 = R.parseEval(solCode.toStdString() + "$pt1");


    // load the curtailment results related to the loaded data
    if(loadCurtailment)
    {
        loadCurResults(solObjectName);
    }
}

void Sub1_result::loadCurResults(QString solObjectName)
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
            Sub1_result::Curtailment_SubD1 curResult;
            lnTs << "[" << i << ",]";
            curResult.cut = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Cut");
            curResult.en_sc = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$En_SC");
            curResult.pet_sc = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Pet_SC");
            curResult.type1_errorRate = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Type_1_Errorrate");
            curResult.type2_errorRate = R.parseEval(curCode.toStdString() + lineNumber.toStdString() + "$Type_2_Errorrate");
            curResult.stoppingRulesNSC = loadStoppingRules(solObjectName, i);
            curtailmentResults->insert(std::pair<int, Curtailment_SubD1>((int)(curResult.cut*100 +0.5), curResult));
            lineNumber.clear();

        }
    }
}

std::vector<Sub1_result::StoppingRule_SubD1>* Sub1_result::loadStoppingRules(QString solObjectName, int row)
{
    RInside &R = RInside::instance();
    QString sRulesCode;
    QTextStream ts(&sRulesCode, QIODevice::ReadWrite);
    std::vector<StoppingRule_SubD1> *sr = new std::vector<StoppingRule_SubD1>();

    // Check if there are stopping rules present in R
    ts << "\"Stoppingrules_for_Row:" << row << "\" %in% names(" << solObjectName << "$Curtailment_Results$'StoppingrulesForID:" << this->iD << "')";
    bool stoppingRulesPresent = R.parseEval(sRulesCode.toStdString());
    sRulesCode.clear();

    // Load stopping rules from R to c++.
    if(stoppingRulesPresent)
    {
        ts << solObjectName << "$Curtailment_Results$'StoppingrulesForID:" << this->iD << "'$'Stoppingrules_for_Row:" << row << "'";
        Rcpp::DataFrame stoppingRulesR = R.parseEval(sRulesCode.toStdString());
        Rcpp::IntegerVector responses_ep1 = stoppingRulesR["Needed_responses_ep1"];
        Rcpp::IntegerVector responses_ep2 = stoppingRulesR["Needed_responses_ep2"];
        Rcpp::IntegerVector observed_patients = stoppingRulesR["Enrolled_patients"];
        size_t i;
        size_t numRules;
        numRules = responses_ep1.length();

        StoppingRule_SubD1 entry;
        for(i = 0; i < numRules; i++)
        {
            entry.t_int = responses_ep1[i];
            entry.u_int = responses_ep2[i];
            entry.enrolled_int = observed_patients[i];
            entry.cp = 0;
            sr->push_back(entry);
        }
        return sr;
    }
    return sr;
}

QDataStream & operator <<(QDataStream &dataStream, const Sub1_result &result)
{
    dataStream << result.getID();
    dataStream << *(result.getName());
    dataStream << result.getR1();
    dataStream << result.getN1();
    dataStream << result.getR();
    dataStream << result.getS();
    dataStream << result.getN();
    dataStream << result.getPc0();
    dataStream << result.getPt0();
    dataStream << result.getPc1();
    dataStream << result.getPt1();
    dataStream << result.getEnP0();
    dataStream << result.getPetP0();
    dataStream << result.getAlpha();
    dataStream << result.getBeta();
    dataStream << result.getAdmissible();
    dataStream << result.getAdmissibleStart();
    dataStream << result.getAdmissibleStop();

    dataStream << result.getUseCurtailment();

    if(result.getUseCurtailment())
    {
        dataStream << result.cut;

        // Write the number of curtailment results to file (used in the deserialization process).
        dataStream << result.curtailmentResults->size();
        std::map<int, Sub1_result::Curtailment_SubD1>::iterator it;
        for(it = result.curtailmentResults->begin(); it != result.curtailmentResults->end(); it++)
        {
            Sub1_result::Curtailment_SubD1 tmp = (Sub1_result::Curtailment_SubD1)((*it).second);
            dataStream << tmp.cut;
            dataStream << tmp.en_sc;
            dataStream << tmp.pet_sc;
            dataStream << tmp.type1_errorRate;
            dataStream << tmp.type2_errorRate;

            // Write the number of stopping rules to file (used in the deserialization process).
            dataStream << tmp.stoppingRulesNSC->size();
            Sub1_result::StoppingRule_SubD1 sr ;
            for(unsigned int i = 0; i < tmp.stoppingRulesNSC->size(); i++)
            {
                sr = tmp.stoppingRulesNSC->at(i);
                dataStream << sr.t_int;
                dataStream << sr.u_int;
                dataStream << sr.enrolled_int;
            }
        }
    }
    return dataStream;
}

QDataStream & operator >>(QDataStream &dataStream, Sub1_result &result)
{
    int tmp = 0;
    double tmpD = 0;
    double tmpD2 = 0;
    bool tmpB = false;
    dataStream >> tmp;
    result.setID(tmp);
    QString *tmpS = new QString();
    dataStream >> *tmpS;
    result.setName(tmpS);
    dataStream >> tmp;
    result.setR1(tmp);
    dataStream >> tmp;
    result.setN1(tmp);
    dataStream >> tmp;
    result.setR(tmp);
    dataStream >> tmp;
    result.setS(tmp);
    dataStream >> tmp;
    result.setN(tmp);
    dataStream >> tmpD;
    result.setPc0(tmpD);
    dataStream >> tmpD;
    result.setPt0(tmpD);
    dataStream >> tmpD;
    result.setPc1(tmpD);
    dataStream >> tmpD;
    result.setPt1(tmpD);
    dataStream >> tmpD;
    result.setEnP0(tmpD);
    dataStream >> tmpD;
    result.setPetP0(tmpD);
    dataStream >> tmpD;
    result.setAlpha(tmpD);
    dataStream >> tmpD;
    result.setBeta(tmpD);
    dataStream >> tmpB;
    dataStream >> tmpD;
    dataStream >> tmpD2;
    if(tmpB)
        result.setAdmissible(tmpD, tmpD2, "");

    dataStream >> tmpB;
    result.setUseCurtailment(tmpB);

    if(result.getUseCurtailment())
    {
        dataStream >> tmp;
        result.setCut(tmp);

        size_t curtailCount;
        dataStream >> curtailCount;

        for(size_t i=0; i < curtailCount; i++)
        {
            Sub1_result::Curtailment_SubD1 curt;
            dataStream >> curt.cut;
            dataStream >> curt.en_sc;
            dataStream >> curt.pet_sc;
            dataStream >> curt.type1_errorRate;
            dataStream >> curt.type2_errorRate;

            size_t stoppingRulesCount;
            dataStream >> stoppingRulesCount;
            curt.stoppingRulesNSC = new std::vector<Sub1_result::StoppingRule_SubD1>();
            for(unsigned int i = 0; i < stoppingRulesCount; i++)
            {
                Sub1_result::StoppingRule_SubD1 sr;
                dataStream >> sr.t_int;
                dataStream >> sr.u_int;
                dataStream >> sr.enrolled_int;
                curt.stoppingRulesNSC->push_back(sr);
            }

            result.addCurtailmentResult(curt);
        }
    }
    return dataStream;
}
