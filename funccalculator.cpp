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


#include "funccalculator.h"

static double fipart(double x)
{
    if(x < 0)
    {
        return ceil(x);
    }
    else
    {
        return floor(x);
    }
}

static double tenPower(double x)
{
     return pow(10, x);
}

FuncCalculator::FuncCalculator(int id, QString funcName, QWidget *parent) : treeCreator(FUNCTION)
{
    funcTree = NULL;
    funcNum = id;
    isExprValidated = areCalledFuncsGood = areIntegrationPointsGood = isParametric = calledFuncsAlreadyChecked = false;
    name = funcName;
    parentWidget = parent;

    addRefFuncsPointers();

    drawState = true;

    for(short i = 0 ; i < 6 ; i++)
    {
        Point pt;
        pt.x = 0;
        pt.y = 0;
        integrationPoints << pt;
    }
}

void FuncCalculator::setColorSaver(ColorSaver *colsaver)
{
    colorSaver = colsaver;
}

void FuncCalculator::setIntegrationPointsList(QList<Point> list)
{
    integrationPoints = list;
}

ColorSaver* FuncCalculator::getColorSaver()
{
    return colorSaver;
}

void FuncCalculator::addRefFuncsPointers()
{
    refFuncs << acos << asin << atan << cos << sin << tan << sqrt
             << log10 << log << fabs << exp << floor << ceil << cosh
             << sinh << tanh << tenPower << tenPower; //tenPower must figure two times for e and E
}

bool FuncCalculator::getDrawState()
{
    return drawState && isFuncValid();
}

bool FuncCalculator::validateExpression(QString expr)
{   
    if(expression != expr)
    {
        if(funcTree != NULL)
            treeCreator.deleteFastTree(funcTree);

        funcTree = treeCreator.getTreeFromExpr(expr, isExprValidated);
        expression = expr;

        integrationPoints.clear();
    }

    calledFuncsAlreadyChecked = false;

    return isExprValidated;
}

void FuncCalculator::setFuncsPointers(QList<FuncCalculator*> otherFuncs)
{
    funcCalculatorsList = otherFuncs;

}

unsigned long int_pow(unsigned long a, unsigned long b)
{
    while(b > 1)
    {
        a *= a;
        b--;
    }

    return a;
}

double FuncCalculator::getAntiderivativeValue(double b, Point A, double k_val)
{   
    double nDiv = fipart(fabs(A.x - b) * 100);
    Point pt1, pt2;
    double newIntegral = 0, oldIntegral = 1, pos;
    double precision = tenPower(-NUM_PREC);

    while(fabs(newIntegral - oldIntegral) > precision)
    {
        oldIntegral = newIntegral;
        newIntegral = 0;
        pos = 1;

        pt1.x = A.x;
        pt1.y = getFuncValue(pt1.x, k_val);

        pt2.x = A.x + (b - A.x)/nDiv;
        pt2.y = getFuncValue(pt2.x, k_val);


        while(pos <= nDiv)
        {
            newIntegral += (pt2.x - pt1.x)*(pt2.y + pt1.y)/2;
            pos++;

            pt1 = pt2;
            pt2.x = A.x + pos*(b - A.x)/nDiv;
            pt2.y = getFuncValue(pt2.x, k_val);
        }
    }

    newIntegral += A.y;

    return newIntegral;

}

double FuncCalculator::getFuncValue(double x, double kValue)
{    
    k = kValue;
    return calculateFromTree(funcTree, x);
}

void FuncCalculator::setDrawState(bool draw)
{
    drawState = draw;
}

double FuncCalculator::getDerivativeValue(double x, double k_val)
{
    k = k_val;
    double y1, y2, y3, y4, a;

    y1 = getFuncValue(x - 2*EPSILON, k);
    y2 = 8*getFuncValue(x - EPSILON, k);
    y3 = 8*getFuncValue(x + EPSILON, k);
    y4 = getFuncValue(x + 2*EPSILON, k);
    a = (y1 - y2 + y3 - y4)/(12*EPSILON);

    return a;
}

void FuncCalculator::setIntegrationPointsValidity(bool state)
{
    areIntegrationPointsGood = state;
}

bool FuncCalculator::checkFuncCallingInclusions()
{
    if(!isExprValidated || !areIntegrationPointsGood)
        return false;

    if(!calledFuncsAlreadyChecked)
    {
        calledFuncsAlreadyChecked = true;

        QList<int> calledFuncs = treeCreator.getCalledFuncs(expression);

        areCalledFuncsGood = !calledFuncs.contains(funcNum);

        if(!areCalledFuncsGood)
            return false;

        for(int i = 0; i < calledFuncs.size() && areCalledFuncsGood; i++)
        {
            areCalledFuncsGood = funcCalculatorsList[calledFuncs[i]]->canBeCalled();
            if(areCalledFuncsGood)
                areCalledFuncsGood = funcCalculatorsList[calledFuncs[i]]->checkFuncCallingInclusions();
        }

        if(!areCalledFuncsGood) // this function calls another function, that is wether invalid or calls back the firs function
            QMessageBox::warning(parentWidget, tr("Erreur"), tr("La fonction ") + name + tr(" appelle une autre fonction invalide, ou qui forme une boucle d'appel infine."));
    }

    return areCalledFuncsGood;

}

void FuncCalculator::setParametric(bool state)
{
    isParametric = state;
}

bool FuncCalculator::isFuncParametric()
{
    return isParametric;
}

bool FuncCalculator::isFuncValid()
{
    return isExprValidated && areCalledFuncsGood && areIntegrationPointsGood;
}

void FuncCalculator::setInvalid()
{
    isExprValidated = false;
}

Range FuncCalculator::getParametricRange()
{
    return kRange;
}

void FuncCalculator::setParametricRange(Range range)
{
    kRange = range;
}

bool FuncCalculator::canBeCalled()
{
    return isExprValidated && areIntegrationPointsGood && areCalledFuncsGood;
}

double FuncCalculator::calculateFromTree(FastTree *tree, double x)
{
    if(tree->type == NOMBRE )
    {
        return *tree->value;
    }
    else if(tree->type == VAR_X || tree->type == VAR_T)
    {
        return x;
    }
    else if(tree->type == PAR_K)
    {
        return k;
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
    else if(REF_FUNC_START <= tree->type && tree->type <= REF_FUNC_END)
    {
        return (*refFuncs[tree->type - REF_FUNC_START])(calculateFromTree(tree->right, x));
    }
    else if(FUNC_START <= tree->type && tree->type <= FUNC_END)
    {
        int id = tree->type - FUNC_START;
        return funcCalculatorsList[id]->getFuncValue(calculateFromTree(tree->right, x), k);
    }
    else if(DERIV_START <= tree->type && tree->type <= DERIV_END)
    {             
        int id = tree->type - DERIV_START;
        return funcCalculatorsList[id]->getDerivativeValue(calculateFromTree(tree->right, x), k);
    }   
    else if(INTEGRATION_FUNC_START <= tree->type && tree->type <= INTEGRATION_FUNC_END)
    {
        int id = tree->type - INTEGRATION_FUNC_START;
        return funcCalculatorsList[id]->getAntiderivativeValue(calculateFromTree(tree->right, x), integrationPoints[id]);
    }
    else if(CONST_Pi_NAMES_START <= tree->type && tree->type <= CONST_Pi_NAMES_END)
    {
        return M_PI;
    }
    else return NAN;
}
