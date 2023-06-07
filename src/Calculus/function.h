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



#ifndef FUNCCALCULATOR_H
#define FUNCCALCULATOR_H

#include "treecreator.h"
#include "colorsaver.h"

class Function : public QObject
{
    Q_OBJECT

public:
    Function(int id, QString funcName);
    ~Function();

    void setFuncsPointers(QList<Function*> otherFuncs);
    void setIntegrationPointsList(QList<Point> list);
    void setParametric(bool state);
    void setParametricRange(Range range);
    void setIntegrationPointsValidity(bool state);
    void setInvalid();

    void setColorSaver(ColorSaver *colsaver);

    ColorSaver* getColorSaver();

    /// \brief checks for errors in other function calls
    /// \returns an error message if ever there's one
    std::optional<QString> checkFuncCallingInclusions();

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

    int funcNum = 0;
    double k = 0;
    bool isExprValidated = false;
    bool isParametric = false;
    bool areCalledFuncsGood = false;
    bool areIntegrationPointsGood= false;
    bool drawState = false;
    bool callLock = false;
    TreeCreator treeCreator;
    FastTree *funcTree = nullptr;
    QString expression, func_name;
    QList<Function*> funcCalculatorsList;
    Range kRange;
    ColorSaver *colorSaver = nullptr;

    QList<Point> integrationPoints;
    QList<double (*)(double)> refFuncs;
};

#endif // FUNCCALCULATOR_H
