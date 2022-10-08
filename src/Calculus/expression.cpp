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

#include "expression.h"
#include "mathworld.h"

#include <cmath>
#include <cstdlib>

using namespace std;

static double tenPower(double x)
{
     return pow(10, x);
}

Expression::Expression(const MathWorld *world)
    : world(world), sub_world(new MathWorld), treeCreator(world, sub_world)
{
}

Expression::~Expression()
{
    delete sub_world;
}

double Expression::calculateExpression(QString expr, bool &ok, double k_val)
{
    ok = true;
    k = k_val;

    ok = checkCalledFuncsValidity(expr);
    if(!ok)
        return std::nan("");

    std::unique_ptr<FastTree> tree = treeCreator.getTreeFromExpr(expr, ok);

    if(!ok)
        return nan("");

    double result = calculateFromTree(tree);

    return result;
}

void Expression::setAdditionnalVarsValues(const std::vector<double> &values)
{
    additionnalVarsValues = values;
}

void Expression::setK(double val)
{
    k = val;
}

bool Expression::checkCalledFuncsValidity(QString expr)
{
    QList<int> calledFuncs = treeCreator.getCalledFuncs(expr);

    if(funcCalculatorsList.isEmpty())
    {
        return calledFuncs.isEmpty();
    }
    else
    {
        bool validity = true;

        for(int i = 0; i < calledFuncs.size() && validity ; i++)
            validity = funcCalculatorsList[calledFuncs[i]]->isFuncValid();

        return validity;
    }
}

void Expression::addRefFuncsPointers()
{
    refFuncs << acos << asin << atan << cos << sin << tan << sqrt
             << log10 << log << fabs << exp << floor << ceil << cosh
             << sinh << tanh << tenPower << tenPower << acosh << asinh
             << atanh << erf << erfc << tgamma << tgamma << cosh
             << sinh << tanh << acosh << asinh << atanh;
}

double Expression::calculateFromTree(const std::unique_ptr<FastTree>& tree, double x)
{
    if(tree->type == NUMBER )
    {
        return tree->value;
    }
    else if(tree->type == PAR_K)
    {
        return k;
    }
    else if(tree->type == VAR_X || tree->type == VAR_T)
    {
        return x;
    }
    else if(tree->type == PLUS)
    {
        return calculateFromTree(tree->left, x) + calculateFromTree(tree->right, x);
    }
    else if(tree->type == MINUS)
    {
        return calculateFromTree(tree->left, x) - calculateFromTree(tree->right, x);
    }
    else if(tree->type == MULTIPLY)
    {
        return calculateFromTree(tree->left, x) * calculateFromTree(tree->right, x);
    }
    else if(tree->type == DIVIDE)
    {
        return calculateFromTree(tree->left, x) / calculateFromTree(tree->right, x);
    }
    else if(tree->type == POW)
    {
        return pow(calculateFromTree(tree->left, x), calculateFromTree(tree->right, x));
    }
    else if(REF_FUNC_START < tree->type && tree->type < REF_FUNC_END)
    {
        return (*refFuncs[tree->type - REF_FUNC_START - 1])(calculateFromTree(tree->right, x));
    }
    else if(FUNC_START < tree->type && tree->type < FUNC_END)
    {
        int id = tree->type - FUNC_START - 1;
        return funcCalculatorsList[id]->getFuncValue(calculateFromTree(tree->right, x), k);
    }
    else if(DERIV_START < tree->type && tree->type < DERIV_END)
    {
        int id = tree->type - DERIV_START - 1;
        return funcCalculatorsList[id]->getDerivativeValue(calculateFromTree(tree->right, x), k);
    }
    else if(tree->type >= ADDITIONNAL_VARS_START)
    {
        return additionnalVarsValues.at(tree->type - ADDITIONNAL_VARS_START);
    }

    else exit(EXIT_FAILURE);
}
