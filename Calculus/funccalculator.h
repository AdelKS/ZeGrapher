/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#ifndef FUNCCALCULATOR_H
#define FUNCCALCULATOR_H

#include "Structures.h"
#include "treecreator.h"
#include "colorsaver.h"

class FuncCalculator : public QObject
{
    Q_OBJECT

public:
    FuncCalculator(int id, QString funcName, QWidget *parent);

    void setFuncsPointers(QList<FuncCalculator*> otherFuncs);
    void setIntegrationPointsList(QList<Point> list);
    void setParametric(bool state);
    void setParametricRange(Range range);
    void setIntegrationPointsValidity(bool state);
    void setInvalid();

    void setColorSaver(ColorSaver *colsaver);

    ColorSaver* getColorSaver();

    bool checkFuncCallingInclusions();

    double getAntiderivativeValue(double b, Point A, double k_val = 0);
    double getFuncValue(double x, double kValue = 0);
    double getDerivativeValue(double x, double k_val = 0);


    bool canBeCalled();
    bool validateExpression(QString expr);    
    bool isFuncValid();
    bool getDrawState();
    bool isFuncParametric();   

    Range getParametricRange();

public slots:    
    void setDrawState(bool draw);

protected:
    double calculateFromTree(FastTree *tree, double x);
    void addRefFuncsPointers();     

    int funcNum;
    double k;
    bool isExprValidated, isParametric, areCalledFuncsGood, areIntegrationPointsGood, drawState, calledFuncsAlreadyChecked;
    TreeCreator treeCreator;
    FastTree *funcTree;
    QString expression, name;
    QList<FuncCalculator*> funcCalculatorsList;
    Range kRange;
    QWidget *parentWidget;  
    ColorSaver *colorSaver;

    QList<Point> integrationPoints;
    QList<double (*)(double)> refFuncs;    
};

#endif // FUNCCALCULATOR_H
