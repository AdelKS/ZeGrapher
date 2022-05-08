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


#include "Calculus/funccalculator.h"

#include <array>

static double tenPower(double x)
{
     return pow(10, x);
}

FuncCalculator::FuncCalculator(int id, QString funcName, QLabel *errorLabel) : treeCreator(ObjectType::FUNCTION)
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

double FuncCalculator::getAntiderivativeValue(double b, Point A, double k_val)
{
    double condition = int_pow(10.0, -NUM_PREC);

    // Adapted from Wikipedia's implementation
    // https://en.wikipedia.org/wiki/Romberg%27s_method

    std::array<double, ROMBERG_MAX_STEPS>  R1, R2; // buffers
    R1.fill(0); R2.fill(0);
    double h = (b-A.x); //step size
    R1[0] = (getFuncValue(b, k_val) + getFuncValue(b, k_val)) * h * .5; // first trapezoidal step

    for (std::size_t i = 1; i < ROMBERG_MAX_STEPS; ++i) {
       h /= 2.;
       double c = 0;
       std::size_t ep = 1 << (i-1); //2^(n-1)
       for (std::size_t j = 1; j <= ep; ++j) {
          c += getFuncValue(A.x + (2*j-1)*h, k_val);
       }
       R2[0] = h*c + .5*R1[0]; //R(i,0)

       for (std::size_t j = 1; j <= i; ++j) {
          double n_k = int_pow(4, j);
          R2[j] = (n_k * R2[j-1] - R1[j-1])/(n_k-1); // compute R(i,j)
       }

       if (i > 1 && fabs(R1[i-1]-R2[i]) < condition) {
          return R2[i-1];
       }

       // swap Rn and Rc as we only need the last row
       R1.swap(R2);
    }
    return R1[ROMBERG_MAX_STEPS-1]; // return our best guess
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

    else return nan("");
}

FuncCalculator::~FuncCalculator()
{
    if(funcTree != nullptr)
        treeCreator.deleteFastTree(funcTree);
}
