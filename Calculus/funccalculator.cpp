/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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


#include "Calculus/funccalculator.h"

static double tenPower(double x)
{
     return pow(10, x);
}

FuncCalculator::FuncCalculator(int id, QString funcName, QLabel *errorLabel) : treeCreator(FUNCTION)
{
    errorMessageLabel = errorLabel;
    funcTree = nullptr;
    funcNum = id;
    isExprValidated = areCalledFuncsGood = areIntegrationPointsGood = isParametric = false;
    name = funcName;

    addRefFuncsPointers();

    drawState = true;
    callLock = false;

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
             << sinh << tanh << tenPower << tenPower << acosh << asinh
             << atanh << erf << erfc << tgamma << tgamma << cosh
             << sinh << tanh << acosh << asinh << atanh; //tenPower must figure two times for e and E
}

bool FuncCalculator::getDrawState()
{
    return drawState && isFuncValid();
}

bool FuncCalculator::validateExpression(QString expr)
{
    if(expression != expr)
    {
        if(funcTree != nullptr)
            treeCreator.deleteFastTree(funcTree);

        funcTree = treeCreator.getTreeFromExpr(expr, isExprValidated);
        expression = expr;

        integrationPoints.clear();
    }

    return isExprValidated;
}

void FuncCalculator::setFuncsPointers(QList<FuncCalculator*> otherFuncs)
{
    funcCalculatorsList = otherFuncs;

}

int int_pow(int a, int b)
{
    if(b==0)
        return 1;

    while(b != 1)
    {
        a *= a;
        b--;
    }

    return a;
}

double FuncCalculator::getAntiderivativeValue(double b, Point A, double k_val)
{
    double a = A.x, fa, fb, hn, result, powResult, diff, condition;

    condition = tenPower(-NUM_PREC);

    fa = getFuncValue(a, k_val);
    fb = getFuncValue(b, k_val);

    if(std::isnan(fa) || std::isinf(fa) || std::isnan(fb) || std::isinf(fb))
        return NAN;

    QList< QList<double> > R;
    QList<double> L1;

    L1 << 0.5*(b-a)*(fa+fb);

    R << L1;

    int i = 0, end = 0;

    do
    {
        i++;
        end = int_pow(2, i-1);
        hn = (b-a)/(double(2*end));
        result = 0;

        for(int j = 1 ; j <= end ; j++)
            result += getFuncValue(a + ((double(2*j)) - 1)*hn, k_val);

        QList<double> L;
        L.reserve(i);
        L << 0.5*R[i-1][0] + hn*result;

        for(int j = 1; j <= i ; j++)
        {
            powResult = int_pow(4, j);
            L << (powResult*L[j-1] - R[i-1][j-1])/(powResult-1);
        }

        R << L;

        diff = fabs(R[i][i] - R[i-1][i-1]);


    }while(diff > condition);

    return R[i][i] + A.y;


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

    QList<int> calledFuncs = treeCreator.getCalledFuncs(expression);

    areCalledFuncsGood = !calledFuncs.contains(funcNum);

    if(!areCalledFuncsGood)
    {
        errorMessageLabel->setText(tr("This function calls itself in its expression."));
        return false;
    }

    callLock = true;
    for(int i = 0; i < calledFuncs.size() && areCalledFuncsGood; i++)
    {
        areCalledFuncsGood = funcCalculatorsList[calledFuncs[i]]->canBeCalled();
        if(areCalledFuncsGood)
            areCalledFuncsGood = funcCalculatorsList[calledFuncs[i]]->checkFuncCallingInclusions();
    }
    callLock = false;

    if(!areCalledFuncsGood)
        errorMessageLabel->setText(tr("This function calls another function that is either undefined or makes an inifite calling loop."));


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
    return isExprValidated && areIntegrationPointsGood && areCalledFuncsGood && !callLock;
}

double FuncCalculator::calculateFromTree(FastTree *tree, double x)
{
    if(tree->type == NUMBER )
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
    else if(INTEGRATION_FUNC_START < tree->type && tree->type < INTEGRATION_FUNC_END)
    {
        int id = tree->type - INTEGRATION_FUNC_START - 1;
        return funcCalculatorsList[id]->getAntiderivativeValue(calculateFromTree(tree->right, x), integrationPoints[id], k);
    }

    else return NAN;
}

FuncCalculator::~FuncCalculator()
{
    if(funcTree != nullptr)
        treeCreator.deleteFastTree(funcTree);
}
