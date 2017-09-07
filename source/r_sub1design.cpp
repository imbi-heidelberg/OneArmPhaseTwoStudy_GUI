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

#include "r_sub1design.h"

R_Sub1Design::R_Sub1Design(): R(RInside::instance())
{
    packageLoaded = false;
    solutionsCalculated = false;
    allResults = new std::vector<Sub1_result*>();
}

R_Sub1Design::R_Sub1Design(long double alpha, long double beta, long double pc0, long double pt0, long double pc1, long double pt1) : R(RInside::instance()),
    alpha(alpha), beta(beta), pc0(pc0), pt0(pt0), pc1(pc1), pt1(pt1)
{
    packageLoaded = false;
    solutionsCalculated = false;
    allResults = new std::vector<Sub1_result*>();
}

bool R_Sub1Design::loadPackage()
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
        packageLoaded = true;
    }
    return packageInstalled && packageLoaded;
}

R_Sub1Design::~R_Sub1Design()
{
    delete allResults;
}

void R_Sub1Design::calculateStudySolutions(bool skipS, bool skipR, bool skipN1, int lowerBorder, int upperBorder)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        // Identify designs which fit to the parameters "alpha", "beta", "pc0", "pt0", "pc1", "pt1" and "maxn".
        QString calculationCode;
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);
        // Set up "sub1"-object and get designs for this object.
        ts << SUB1_OBJECT <<" <- setupSub1Design( alpha =" << this->alpha <<", beta = " << this->beta << ", pc0 = " << this->pc0 << ", pt0 = " << this->pt0
           << ", pc1 = " << this->pc1 << ", pt1 = " << this->pt1 << "); \n";
        //ts << SUB1_OBJECT << "$setMaxN(" << this->maxn << "); \n";

        ts << SOLNAME_SUB1 << "<- getSolutionsSub1(sub1 = " << SUB1_OBJECT << ", skipS = " << (skipS ? "T":"F") << ", skipR = " << (skipR ? "T":"F") << ", skipN1 = " << (skipN1 ? "T":"F")
           << ", lowerBorder = " << lowerBorder << ", upperBorder = " << upperBorder << ", useCurtailment = F, curtailAll = F)";
        qDebug() << calculationCode;
        R.parseEvalQ(calculationCode.toStdString());
        // Get number of found designs and read them back to c++.
        calculationCode.clear();
        ts << "nrow(" << SOLNAME_SUB1 << "$Solutions)";
        qDebug() << calculationCode;
        int numOfResults = R.parseEval(calculationCode.toStdString());
        int i;
        allResults->clear();
        for(i = 1; i <= numOfResults; i++)
        {
            Sub1_result *res = new Sub1_result();
            res->loadResultFromR(SOLNAME_SUB1, i, false);
            allResults->push_back(res);
        }
    }
}

int R_Sub1Design::getNumberOfResults(){return allResults->size();}


Sub1_result* R_Sub1Design::getResult(int id){return allResults->at(id);}

void R_Sub1Design::calculateSC(int resID, double cut, int reps, bool all)
{
    QString calcSC_Code;
    QTextStream ts(&calcSC_Code, QIODevice::ReadWrite);
    // Invoke "calculateSC" in R to calculate (non)stochastic curtailment for the design with the ID == "resID".
    ts << SUB1_OBJECT << "$calculateSC(" << resID << ", " << cut << ", " << reps << ", " << (all ? "T": "F") << "); \n";
    ts << SOLNAME_SUB1 << "<- toDataframe( "<< SUB1_OBJECT << ", T" << "); \n";
    R.parseEvalQ(calcSC_Code.toStdString());

    Sub1_result* res = allResults->at(resID);
    // Load curtailment results from R back to c++.
    res->loadCurResults(SOLNAME_SUB1);
}

double R_Sub1Design::get_UMVUE_ep1(int t, int r1, int n1, int n)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        double umvue = 0;
        QString rCode;
        QString resultName = "umvue_ep1";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "get_UMVUE_GMS(" << t << ", " << r1 << ", " << n1 << ", " << n <<")";
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

double R_Sub1Design::get_UMVUE_ep2(int t, int u, int r1, int n1, int n)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        double umvue = 0;
        QString rCode;
        QString resultName = "umvue_ep2";
        QTextStream ts(&rCode, QIODevice::ReadWrite);
        ts << resultName << " <- " << "get_UMVUE_GMS_subset_second_total(" << t << ", " << u << ", " << r1 << ", " << n1 << ", " << n <<")";
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

int R_Sub1Design::aproximateMaxN()
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString calculationCode;
        QString r_maxN = "maxN";
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);
        // Set up a "sub1"-object.
        ts << SUB1_OBJECT <<" <- setupSub1Design( alpha =" << this->alpha <<", beta = " << this->beta << ", pc0 = " << this->pc0 << ", pt0 = " << this->pt0
           << ", pc1 = " << this->pc1 << ", pt1 = " << this->pt1 << ");";
        qDebug() << calculationCode;
        R.parseEvalQ(calculationCode.toStdString());
        calculationCode.clear();
        // Aproximate maxn.
        ts << r_maxN << " <- " << SUB1_OBJECT << "$aproximateMaxN()";
        qDebug() << calculationCode;
        R.parseEvalQ(calculationCode.toStdString());
        // Read result back from R to c++.
        Rcpp::NumericVector tmp = R[r_maxN.toStdString()];

        int maxN = tmp[0];
        return maxN;

    }
    else
    {
        return -1;
    }
}

double R_Sub1Design::get_conditionalPower(int t, int u, int enrolled, int r1, int n1, int r, int s, int n, double pc1, double pt1)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString calculationCode;
        QString r_cp = "cp";
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);

        // Get the conditional power by invoking "get_conditionalPower".
        ts << r_cp << " <- get_conditionalPower( t = " << t << ", u = " << u << ", enrolled = " << enrolled << ", r1 = " << r1
           << ", n1 = " << n1 << ", r = " << r << ", s = " << s << ", n = " << n << ", pc1 = " << pc1 << ", pt1 = " << pt1 << ")";

        qDebug() << calculationCode;
        R.parseEvalQ(calculationCode.toStdString());
        // Load result from R to c++.
        Rcpp::NumericVector tmp = R[r_cp.toStdString()];

        double cp = tmp[0];
        return cp;
    }
    return -1;
}

double R_Sub1Design::get_pValue(int t, int u, int r1, int n1, int n, double pc0, double pt0)
{
    if(!packageLoaded) // load "OneArmPhaseTwo"-package if not allready loaded
        this->loadPackage();
    if(packageLoaded)
    {
        QString calculationCode;
        QString r_pValue = "pValue";
        QTextStream ts(&calculationCode, QIODevice::ReadWrite);

        ts << r_pValue << " <- get_p_exact_subset( t = " << t << ", u = " << u << ", r1 = " << r1
           << ", n1 = " << n1 << ", n = " << n << ", pc0 = " << pc0 << ", pt0 = " << pt0 << ")";

        qDebug() << calculationCode;
        R.parseEvalQ(calculationCode.toStdString());
        Rcpp::NumericVector tmp = R[r_pValue.toStdString()];

        double pValue = tmp[0];
        return pValue;
    }
    return -1;
}


void R_Sub1Design::setAlpha(double alpha){this->alpha = alpha;}
double R_Sub1Design::getAlpha(){return alpha;}
void R_Sub1Design::setBeta(double beta){this->beta = beta;}
double R_Sub1Design::getBeta(){return beta;}
void R_Sub1Design::setPc0(double pc0){this->pc0 = pc0;}
double R_Sub1Design::getPc0(){return pc0;}
void R_Sub1Design::setPt0(double pt0){this->pt0 = pt0;}
double R_Sub1Design::getPt0(){return pt0;}
void R_Sub1Design::setPc1(double pc1){this->pc1 = pc1;}
double R_Sub1Design::getPc1(){return pc1;}
void R_Sub1Design::setPt1(double pt1){this->pt1 = pt1;}
double R_Sub1Design::getPt1(){return pt1;}

int R_Sub1Design::getMinimaxPos()
{
    QString tmp = "tmp";
    QString objectName;
    QTextStream ts(&objectName, QIODevice::ReadWrite);
    ts << tmp << " <- " << SUB1_OBJECT << "$miniMaxPos";
    R.parseEvalQ(objectName.toStdString());
    int pos = R[tmp.toStdString()];
    return pos;
}

int R_Sub1Design::getOptimalPos()
{
    QString tmp = "tmp";
    QString objectName;
    QTextStream ts(&objectName, QIODevice::ReadWrite);
    ts << tmp << " <- " << SUB1_OBJECT << "$optimalPos";
    R.parseEvalQ(objectName.toStdString());
    int pos = R[tmp.toStdString()];
    return pos;
}
