// Main-file wich establishes the link between CPP and R
// and also starts the GUI (MainWindow defined in "mainwindow.h")
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


#include <QtGui/QApplication>
#include "mainwindow.h"
#include <RInside.h>

int main(int argc, char *argv[])
{
    RInside *R;
    char *dummy = (char*)" ";
    R = new RInside(0,&dummy);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();    

    int returnVal = a.exec();
    delete R;

    return returnVal;
}
