/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

#ifndef EXPRCALCULATOR_H
#define EXPRCALCULATOR_H

#include "treecreator.h"
#include "function.h"

class ExprCalculator
{
public:

    explicit ExprCalculator(bool allowK = false, QList<Function*> otherFuncs = QList<Function*>());

    double calculateExpression(QString expr, bool &ok, double k_val = 0);
    void setAdditionnalVarsValues(const std::vector<double> &values);
    void setK(double val);

    double calculateFromTree(FastTree *tree, double x = 0);
    bool checkCalledFuncsValidity(QString expr);

protected:
    void addRefFuncsPointers();

    double k;
    TreeCreator treeCreator;
    QList<Function*> funcCalculatorsList;
    QList<double (*)(double)> refFuncs;
    std::vector<double> additionnalVarsValues;
};

#endif // EXPRCALCULATOR_H
