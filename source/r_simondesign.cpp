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

#include "r_simondesign.h"

R_simondesign::R_simondesign() :  R(RInside::instance())
{
    allResults = new std::vector<Result*>();
    packageLoaded = false;
    solutionsCalculated = false;
    this->alpha = 0;
    this->beta = 0;
    this->p0 = 0;
    this->p1 = 0;
    miniMaxPos = 0;
    optimalPos = 0;
}

R_simondesign::R_simondesign(double alpha, double beta, double p0, double p1) : R(RInside::instance())
{
    this->alpha = alpha;
    this->beta = beta;
    this->p0 = p0;
    this->p1 = p1;
    miniMaxPos = 0;
    optimalPos = 0;
    allResults = new std::vector<Result*>();

    packageLoaded = false;
    solutionsCalculated = false;
}

R_simondesign::~R_simondesign()
{
    delete allResults;
}

bool R_simondesign::loadPackage()
{
    // Is the "OneArmPhaseTwo"-package installed?
    QString loadePackageCode;
    QTextStream ts(&loadePackageCode, QIODevice::ReadWrite);
    ts << "\"" << PACKAGE_NAME << "\" %in% rownames(installed.packages())";
    bool packageInstalled = false;
    packageInstalled = R.parseEval(loadePackageCode.toStdString());
    // If it is installed try to load it.
    if(packageInstalled)
    {
        loadePackageCode.clear();
        ts << "library(\"" << PACKAGE_NAME << "\")";
        R.parseEvalQ(loadePackageCode.toStdString());
        packageLoaded = true; // Remember that the package is loaded.
    }
    return packageInstalled && packageLoaded;
}

void R_simondesign::calculateStudySolutions()
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        // Identify designs which fit to the parameters "alpha", "beta", "p0", "p1" and "maxn".
        QString calculationCode;
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);
        // Set up "simon"-object and get designs for this object.
        ts << SIMON_OBJECT <<" <- setupSimon( alpha =" << this->alpha <<", beta = " << this->beta << ", p0 = " << this->p0 << ", p1 = " << this->p1 << "); \n";
        ts << SIMON_OBJECT << "$setMaxN(" << this->maxn << "); \n";
        ts << SOLNAME << "<- getSolutions(simon = " << SIMON_OBJECT << ", useCurtailment = F, curtail_All = F)";
        R.parseEvalQ(calculationCode.toStdString());
        // Get number of found designs and read them back to c++.
        calculationCode.clear();
        ts << "nrow(" << SOLNAME << "$Solutions)";
        int numOfResults = R.parseEval(calculationCode.toStdString());
        int i;
        allResults->clear();
        for(i = 1; i <= numOfResults; i++)
        {
            Result *res = new Result();
            res->loadResultFromR(SOLNAME, i, false);
            allResults->push_back(res);
        }
    }
}

void R_simondesign::calculateSC(int resID, double cut, int reps, bool all)
{
    QString calcSC_Code;
    QTextStream ts(&calcSC_Code, QIODevice::ReadWrite);

    // Invoke "calculateSC" in R to calculate (non)stochastic curtailment for the design with the ID == "resID".
    ts << SIMON_OBJECT << "$calculateSC(" << resID << ", " << cut << ", " << reps << ", " << (all ? "T": "F") << "); \n";
    ts << SOLNAME << "<- toDataframe( "<< SIMON_OBJECT << ", T" << "); \n";
    qDebug() << calcSC_Code;
    R.parseEvalQ(calcSC_Code.toStdString());

    Result* res = allResults->at(resID);
    // Load curtailment results from R back to c++.
    res->loadCurResults(SOLNAME);
}

int R_simondesign::getMiniMaxPos()
{
    QString tmp = "tmp";
    QString objectName;
    QTextStream ts(&objectName, QIODevice::ReadWrite);
    ts << tmp << " <- " << SIMON_OBJECT << "$miniMaxPos";
    R.parseEvalQ(objectName.toStdString());
    int pos = R[tmp.toStdString()];
    return pos;
}

int R_simondesign::getOptimalPos()
{
    QString tmp = "tmp";
    QString objectName;
    QTextStream ts(&objectName, QIODevice::ReadWrite);
    ts << tmp << " <- " << SIMON_OBJECT << "$optimalPos";
    R.parseEvalQ(objectName.toStdString());
    int pos = R[tmp.toStdString()];
    return pos;
}

void R_simondesign::resultToR(Result *res, QString name)
{
    QString rCode;
    QTextStream ts(&rCode, QIODevice::ReadWrite);
    // Create a data.frame in R with one row containing all design parameters stored in "res".
    ts << name << "<- data.frame(r1 = " << res->r1 ;
    ts << ", n1 = " << res->n1;
    ts << ", r = " << res->r;
    ts << ", n = " << res->n;
    ts << ", p0 = " << res->p0;
    ts << ", p1 = " << res->p1;
    ts << ", enP0 = " << res->enP0;
    ts << ", enP1 = " << res->enP1;
    ts << ", petP0 = " << res->petP0;
    ts << ", petP1 = " << res->petP1;
    ts << ", Alpha = " << res->alpha;
    ts << ", Beta = " << res->beta;
    if(res->admissible)
    {
        ts << ", Admissible = 1";
        ts << ", Admiss_Start = " << res->admissibleStart;
        ts << ", Admiss_End = " << res->admissibleStop;
    }
    else
    {
        ts << ", Admissible = 0";
        ts << ", Admiss_Start = NA";
        ts << ", Admiss_End = NA";
    }
    ts << ");";

    R.parseEvalQ(rCode.toStdString());
}

void R_simondesign::calculate_CE_proportional(Result *res, QString name)
{
    QString rCode;
    QTextStream ts(&rCode, QIODevice::ReadWrite);
    QString objectName = "ceProp";
    // Invoke "getD_proportionally" to calculate the conditional error for all possible outcomes
    // at the interim analysis (different number of responses) spending "rest alpha"
    // (difference between nominal alpha level and actual alpha level) proportionally and store the
    // results in an data.frame named "ceProp".
    ts << objectName << "<- getD_proportionally(" << name << ", " << this->alpha << ");";
    R.parseEvalQ(rCode.toStdString());

    // Load results from R to c++.
    Rcpp::DataFrame df = R[objectName.toStdString()];

    Rcpp::IntegerVector k = df["k"];
    Rcpp::NumericVector ce = df["ce"];
    size_t size = k.length();
    int ki;
    double cei;
    for(size_t i = 0; i < size; i++)
    {
        ki = k[i];
        cei = ce[i];
        res->flexibleAdaptionProp->insert(std::pair<int,double>(ki,cei));
    }
}

void R_simondesign::calculate_CE_equally(Result *res, QString name)
{
    QString rCode;
    QTextStream ts(&rCode, QIODevice::ReadWrite);
    QString objectName = "ceEqua";
    // Invoke "getD_equally" to calculate the conditional error for all possible outcomes
    // at the interim analysis (different number of responses) spending "rest alpha"
    // (difference between nominal alpha level and actual alpha level) equally and store the
    // results in an data.frame named "ceEqua".
    ts << objectName <<" <- getD_equally(" << name << ", " << this->alpha << ");";
    R.parseEvalQ(rCode.toStdString());

    // Load results from R to c++.
    Rcpp::DataFrame df = R[objectName.toStdString()];

    Rcpp::IntegerVector k = df["k"];
    Rcpp::NumericVector ce = df["ce"];
    size_t size = k.length();
    int ki;
    double cei;
    for(size_t i = 0; i < size; i++)
    {
        ki = k[i];
        cei = ce[i];
        res->flexibleAdaptionEqua->insert(std::pair<int,double>(ki,cei));
    }
}

void R_simondesign::calculate_CE_tolowest(Result *res, QString name)
{
    QString rCode;
    QTextStream ts(&rCode, QIODevice::ReadWrite);
    QString objectName = "ceToOne";
    // Invoke "getD_distributeToOne" to calculate the conditional error for all possible outcomes
    // at the interim analysis (different number of responses) spending the "rest alpha"
    // (difference between nominal alpha level and actual alpha level) only to increase the worst case
    // (smallest conditional error value that is not equal to 0) and store the results
    // in a data.frame named "ceToOne"
    ts << objectName <<" <- getD_distributeToOne(" << name << ", " << this->alpha << ");";
    R.parseEvalQ(rCode.toStdString());

    // Load results from R to c++.
    Rcpp::DataFrame df = R[objectName.toStdString()];

    Rcpp::IntegerVector k = df["k"];
    Rcpp::NumericVector ce = df["ce"];
    size_t size = k.length();
    int ki;
    double cei;
    for(size_t i = 0; i < size; i++)
    {
        ki = k[i];
        cei = ce[i];
        res->flexibleAdaptionToOne->insert(std::pair<int,double>(ki,cei));
    }
}

void R_simondesign::calculate_CE_none(Result *res, QString name)
{
    QString rCode;
    QTextStream ts(&rCode, QIODevice::ReadWrite);
    QString objectName = "ceNone";
    // Invoke "getD_none" to calculate the conditional error for all possible outcomes at the interim analysis
    // (different number of responses) using no "rest alpha" spending (difference between nominal alpha level and actual alpha level)
    // and store the results in a data.frame named "ceNone".
    ts << objectName <<" <- getD_none(" << name << ");";
    R.parseEvalQ(rCode.toStdString());
    // Load results from R to c++.
    Rcpp::DataFrame df = R[objectName.toStdString()];

    Rcpp::IntegerVector k = df["k"];
    Rcpp::NumericVector ce = df["ce"];
    size_t size = k.length();
    int ki;
    double cei;
    for(size_t i = 0; i < size; i++)
    {
        ki = k[i];
        cei = ce[i];
        res->flexibleAdaptionNone->insert(std::pair<int,double>(ki,cei));
    }
}

void R_simondesign::calcultateCE(int resID)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        Result *res = this->allResults->at(resID);
        QString name = "design";
        // Create a data.frame in R with one row named "design" containing all design parameters stored in "res".
        // Afterwards the effect of the "flexible extension" (rest alpha spending) for that design are calculated.
        this->resultToR(res, name);
        calculate_CE_proportional(res, name);
        calculate_CE_equally(res, name);
        calculate_CE_tolowest(res, name);
        calculate_CE_none(res, name);
    }
}

double R_simondesign::getCP_flex(int n2, Result *res, int responses, double alpha)
{
    double result = -1;
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString name = "design";
        QString resultName = "resCP";
        this->resultToR(res, name);
        QString rCode;
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        // Calculate the conditional power in R (if the sample size of the second stage is altered to "n2").
        ts << resultName << " <- " << "getCP(" << n2 << ", " << res->p1 <<", " << name << ", " << responses << ", 3, " << alpha << ")";
        R.parseEvalQ(rCode.toStdString());
        // Load results from R to c++.
        Rcpp::NumericVector cp =  R[resultName.toStdString()];
        result = cp[0];
        return result;
    }else{
        return result;
    }
}

double R_simondesign::getConditionalPower(int k, int numPat, int r1, int n1, int r, int n, double p1)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString calculationCode;
        QString r_cp = "cp";
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);

        // Get the conditional power by invoking "getConditionalPower".
        ts << "tmp_s <- setupSimon();";
        ts << r_cp << " <- "  << "tmp_s$getConditionalPower(" << k << "," << numPat << ","
           << r1 << "," << n1 << "," << r << "," << n << "," << p1 << ")";

        //ts << r_cp << " <- getCP_simon( k = " << k << ", numPat = " << numPat << ", r1 = " << r1
        //   << ", n1 = " << n1 << ", r = " << r << ", n = " << n << ", p1 = " << p1 << ")";

        // Load result from R to c++.
        R.parseEvalQ(calculationCode.toStdString());
        Rcpp::NumericVector tmp = R[r_cp.toStdString()];

        double cp = tmp[0];
        return cp;
    }
    return -1;
}

int R_simondesign::getN(double cp, Result *res, int responses, double alpha)
{
    int n = -1;
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString name = "design";
        QString resultName = "resN2";
        this->resultToR(res, name);
        QString rCode;
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "getN2(" << cp << ", " << res->p1 <<", " << name << ", " << responses << ", 3, " << alpha << ")";
        //qDebug() << rCode;
        R.parseEvalQ(rCode.toStdString());
        Rcpp::IntegerVector n2 =  R[resultName.toStdString()];
        n = res->n1 + n2[0];
        return n;
    }else{
        return n;
    }

}

double R_simondesign::get_UMVUE(int k, int r1, int n1, int n)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        double umvue = 0;
        QString rCode;
        QString resultName = "umvue";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "get_UMVUE_GMS(" << k << ", " << r1 << ", " << n1 << ", " << n <<")";
        R.parseEvalQ(rCode.toStdString());

        Rcpp::NumericVector umvueR = R[resultName.toStdString()];
        umvue = umvueR[0];

        return umvue;
    }
    else
    {
        return -1;
    }
}

double R_simondesign::get_pValue(int k, int r1, int n1, int n, double p0)
{
    if(!packageLoaded)  // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        double pValue = 0;
        QString rCode;
        QString resultName ="pValue";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "get_p_KC(" << k << ", " << r1 << ", " << n1 << ", " << n << ", " << p0 << ")";
        R.parseEvalQ(rCode.toStdString());

        Rcpp::NumericVector pValueR = R[resultName.toStdString()];
        pValue = pValueR[0];

        return pValue;
    }
    else
    {
        return -1;
    }
}

double* R_simondesign::get_CI(int k, int r1, int n1, int n, double alpha, int percision)
{
    if(!packageLoaded)  // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        double *ci = new double[2];
        QString rCode;
        QString resultName ="ci";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "as.numeric(get_CI(" << k << ", " << r1 << ", " << n1 << ", " << n << ", " << alpha << ", " << percision << "))";
        R.parseEvalQ(rCode.toStdString());

        Rcpp::NumericVector ciR = R[resultName.toStdString()];
        ci[0] = ciR[0];
        ci[1] = ciR[1];

        return ci;
    }
    else
    {
        return NULL;
    }
}


int R_simondesign::getR2forFlex(double ce, double p0, int n2)
{
    int r2 = -1;
    if(!packageLoaded)  // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString rCode;
        QString resultName ="r2";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "as.numeric(get_r2_flex(" << ce << ", " << p0 << ", " << n2 << "))";
        R.parseEvalQ(rCode.toStdString());

        Rcpp::NumericVector r2R = R[resultName.toStdString()];
        r2 = r2R[0];
        return r2;
    }
    return r2;
}

double R_simondesign::getPvalueSingelStage(int k, double p0, int n2)
{
    double pValue = -1;
    if(!packageLoaded)  // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString rCode;
        QString resultName ="pValue";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "as.numeric(getP(" << k << ", " << p0 << ", " << n2 << "))";
        R.parseEvalQ(rCode.toStdString());

        Rcpp::NumericVector pValueR = R[resultName.toStdString()];
        pValue = pValueR[0];
        return pValue;
    }
    return pValue;
}

void R_simondesign::setAlpha(double a){ this->alpha = a;}

void R_simondesign::setBeta(double b){ this->beta = b;}

void R_simondesign::setP0(double p0){ this->p0 = p0;}

void R_simondesign::setP1(double p1){ this->p1 = p1;}

void R_simondesign::setMaxN(int maxN){ this->maxn = maxN;}

int R_simondesign::aproximateMaxN()
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString calculationCode;
        QString r_maxN = "maxN";
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);
        // Set up a "simon"-object.
        ts << SIMON_OBJECT <<" <- setupSimon( alpha =" << this->alpha <<", beta = " << this->beta << ", p0 = " << this->p0
           << ", p1 = " << this->p1 << ");";
        R.parseEvalQ(calculationCode.toStdString());
        // Aproximate maxn.
        calculationCode.clear();
        ts << r_maxN << " <- " << SIMON_OBJECT << "$aproximateMaxN()";
        R.parseEvalQ(calculationCode.toStdString());
        Rcpp::NumericVector tmp = R[r_maxN.toStdString()];

        int maxN = tmp[0];
        return maxN;
    }
    else
    {
        return -1;
    }
}

Result* R_simondesign::getResult(int id){ return allResults->at(id);}

int R_simondesign::getNumberOfResults(){return allResults->size();}
