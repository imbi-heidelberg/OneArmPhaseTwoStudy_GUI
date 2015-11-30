# This file is part of OneArmPhaseTwoStudy.
#
# OneArmPhaseTwoStudy is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# OneArmPhaseTwoStudy is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details (http://www.gnu.org/licenses/).
QT       += core gui

TARGET = OneArmPhaseTwoStudy_GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    result.cpp \
    newstudyfirstpage.cpp \
    newstudyoverview.cpp \
    study.cpp \
    studyprogresspage.cpp \
    rwidget.cpp \
    r_simondesign.cpp \
    studyevaluationpage.cpp \
    r_sub1design.cpp \
    sub1_result.cpp \
    idtablewidgetitem.cpp \
    designpage.cpp

HEADERS  += mainwindow.h \
    result.h \
    newstudyfirstpage.h \
    newstudyoverview.h \
    study.h \
    studyprogresspage.h \
    rwidget.h \
    r_simondesign.h \
    studyevaluationpage.h \
    r_sub1design.h \
    sub1_result.h \
    idtablewidgetitem.h \
    designpage.h

FORMS    += mainwindow.ui \
    newstudyfirstpage.ui \
    newstudyoverview.ui \
    studyprogresspage.ui \
    rwidget.ui \
    studyevaluationpage.ui \
    designpage.ui

## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
R_HOME = 		$$system(R RHOME)

## include headers and libraries for R
RCPPFLAGS = 		$$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS = 		$$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS = 		$$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK = 		$$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
#RRPATH =		-Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
RCPPINCL = 		$$system($$R_HOME/bin/Rscript -e Rcpp:::CxxFlags())
RCPPLIBS = 		$$system($$R_HOME/bin/Rscript -e Rcpp:::LdFlags())

## for some reason when building with Qt we get this each time
##   /usr/local/lib/R/site-library/Rcpp/include/Rcpp/module/Module_generated_ctor_signature.h:25: warning: unused parameter ‘classname
## so we turn unused parameter warnings off
## no longer needed with Rcpp 0.9.3 or later
#RCPPWARNING =		-Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL = 		$$system($$R_HOME/bin/Rscript -e RInside:::CxxFlags())
RINSIDELIBS = 		$$system($$R_HOME/bin/Rscript -e RInside:::LdFlags())

## compiler etc settings used in default make rules
QMAKE_CXXFLAGS +=	$$RCPPWARNING $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL
QMAKE_LIBS +=           $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS
