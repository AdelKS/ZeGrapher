/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
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



#ifndef FUNCTION_H
#define FUNCTION_H

#include "treecreator.h"
#include "colorsaver.h"
#include "expression.h"

class FuncCalculator : public Expression
{
    Q_OBJECT

public:
    FuncCalculator(int id, QString funcName, QLabel *errorLabel);

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
    double calculateFromTree(const std::unique_ptr<FastTree> &tree, double x);
    void addRefFuncsPointers();

    int funcNum;
    double k;
    bool isExprValidated, isParametric, areCalledFuncsGood, areIntegrationPointsGood, drawState, callLock;
    TreeCreator treeCreator;
    std::unique_ptr<FastTree> funcTree;
    QString expression, name;
    QList<FuncCalculator*> funcCalculatorsList;
    Range kRange;
    ColorSaver *colorSaver;
    QLabel *errorMessageLabel;

    QList<Point> integrationPoints;
    QList<double (*)(double)> refFuncs;
};

#endif // FUNCTION_H
