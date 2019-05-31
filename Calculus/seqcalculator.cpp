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





#include "Calculus/seqcalculator.h"

static double tenPower(double x)
{
     return pow(10, x);
}

SeqCalculator::SeqCalculator(int id, QString name, QLabel *errorLabel) : treeCreator(ObjectType::SEQUENCE), firstValsTreeCreator(ObjectType::NORMAL_EXPR)
{   
    seqNum = id;
    isExprValidated = isValid = isKRangeValid = blockCalculatingFromTree = false;
    errorMessageLabel = errorLabel;

    areFirstValsValidated = true;    
    nMin = kPos = 0;
    drawsNum = 1;
    custom_k = 0;
    k = 0;
    drawState = true;
    seqTree = nullptr;

    addRefFuncsPointers();

    firstValsTreeCreator.allow_k(true);
    kRange.start = 0;
    kRange.step = 1;
    kRange.end = 0.5;

    seqsNames << "(u<sub>n</sub>)" << "(v<sub>n</sub>)" << "(l<sub>n</sub>)" << "(w<sub>n</sub>)" << "(q<sub>n</sub>)" << "(z<sub>n</sub>)";

    seqValues << QList<double>();

    seqName =  name;
}

bool SeqCalculator::validateFirstValsExpr(QString expr)
{  
    firstValsExpr = expr;
    areFirstValsValidated = validateSeqFirstValsTrees();
    seqValues.clear();
    drawsNum = 1;

    if(!areFirstValsValidated)
        errorMessageLabel->setText(tr("NB: First values must be separated by ';'"));

    return areFirstValsValidated;
}

bool SeqCalculator::validateSeqExpr(QString expr)
{
    expression = expr;
    drawsNum = 1;
    seqValues.clear();

    if(seqTree != nullptr)
        treeCreator.deleteFastTree(seqTree);

    seqTree = treeCreator.getTreeFromExpr(expr, isExprValidated);

    return isExprValidated;
}

void SeqCalculator::setDrawState(bool draw)
{
    drawState = draw;
}

void SeqCalculator::setParametricInfo(bool parametric, Range parRange)
{
    isParametric = parametric;
    kRange = parRange;
    drawsNum = trunc((kRange.end - kRange.start)/kRange.step) + 1;

    isKRangeValid = drawsNum > 0;

    if(isKRangeValid)
        updateSeqValuesSize();
}

int SeqCalculator::getDrawsNum()
{
    return drawsNum;
}

void SeqCalculator::set_nMin(int val)
{
    nMin = val;
    seqValues.clear();
    updateSeqValuesSize();
}

bool SeqCalculator::isSeqParametric()
{
    return isParametric;
}

Range SeqCalculator::getKRange()
{
    return kRange;
}

void SeqCalculator::setColorSaver(ColorSaver *colsaver)
{
    colorSaver = colsaver;
}

ColorSaver* SeqCalculator::getColorSaver()
{
    return colorSaver;
}

int SeqCalculator::get_nMin()
{
    return nMin;
}

bool SeqCalculator::getDrawState()
{
    return drawState && isSeqValid();
}

bool SeqCalculator::isSeqValid()
{
    return isExprValidated && areFirstValsValidated && isValid && isKRangeValid;
}

void SeqCalculator::setInvalid()
{
    isExprValidated = false;
}

void SeqCalculator::setFuncsPointers(QList<FuncCalculator*> otherFuncs)
{
    funcCalculatorsList = otherFuncs;
}

void SeqCalculator::setSeqsPointers(QList<SeqCalculator*> otherSeqs)
{
    seqCalculatorsList = otherSeqs;
}

bool SeqCalculator::checkByCalculatingFirstValuesTrees()
{
    if(!isExprValidated || !areFirstValsValidated)
        return false;

    isValid = calculateAndSaveFirstValuesTrees();

    if(!isValid)
        errorMessageLabel->setText(tr("An error occured while trying to calculate the entered first values."));

    return isValid;
}

bool SeqCalculator::checkByCalculatingValues()
{
    if(!isExprValidated || !areFirstValsValidated)
        return false;

    isValid = saveSeqValues(3*seqValues[0].size() + 10 - nMin);
    blockCalculatingFromTree = false;

    return isValid;
}

double SeqCalculator::getCustomSeqValue(double n, bool &ok, double k_value)
{
    if( n < nMin || n > MAX_SAVED_SEQ_VALS)
        return nan("");

    double index = (k_value - kRange.start)/kRange.step;

    if(0 <= index && index < drawsNum && index == floor(index))
        return getSeqValue(n, ok, index);

    if(custom_k != k_value)
    {
        custom_k = k_value;
        seqValues[drawsNum].clear();
    }


    if(n-nMin >= seqValues[drawsNum].size())
        ok = saveCustomSeqValues(n);

    if(!ok)
        return nan("");

    return seqValues[drawsNum][n-nMin];
}

bool SeqCalculator::saveCustomSeqValues(double nMax)
{
    if(blockCalculatingFromTree)
        return false;

    blockCalculatingFromTree = true;

    if(nMax-nMin > MAX_SAVED_SEQ_VALS)
        nMax = MAX_SAVED_SEQ_VALS + nMin;

    double result;
    bool ok = true;
    kPos = drawsNum;
    k = custom_k;

    if(seqValues[kPos].size() == 0)
    {
        for(int i = 0; i < firstValsTrees.size(); i++)
        {
            result = calculateFromTree(firstValsTrees[i],i, ok);

            if(!ok)
                return false;

            seqValues[kPos] << result;
        }
    }

    for(int n = seqValues[kPos].size() + nMin; n <= nMax + nMin; n++)
    {
        result = calculateFromTree(seqTree, n, ok);

        if(!ok)
            return false;

        seqValues[kPos] << result;
    }

    blockCalculatingFromTree = false;

    return true;
}

double SeqCalculator::getSeqValue(double n, bool &ok, int index_k)
{   
    if(n < nMin || seqValues[0].size() > MAX_SAVED_SEQ_VALS)
        return nan("");

    if(n-nMin >= seqValues[0].size())
    {
        ok = isValid = saveSeqValues(n);
        blockCalculatingFromTree = false;
    }

    if(!ok)
        return nan("");

    if(0 <= index_k && index_k < drawsNum)
    {
        double res = seqValues[index_k][n-nMin];
        return res;
    }
    else return nan("");
}

void SeqCalculator::updateSeqValuesSize()
{
    int size = trunc((kRange.end - kRange.start)/kRange.step) + 2;

    for(int i = seqValues.size() ; i < size ; i++)
        seqValues << QList<double>();
}

bool SeqCalculator::saveSeqValues(double nMax)
{
    if(blockCalculatingFromTree)
    {
        errorMessageLabel->setText(tr("Invalid crossed recursion between this sequence and the other(s) it calls in its expression."));

        return false;
    }

    blockCalculatingFromTree = true;
    bool ok = true;

    if(nMax > MAX_SAVED_SEQ_VALS + nMin)
        nMax = MAX_SAVED_SEQ_VALS + nMin;

    double result;

    k = kRange.start;

    for(kPos = 0; kPos < drawsNum; kPos++)
    {
        for(int n = seqValues[kPos].size() - nMin; n <= nMax ; n++)
        {
            result = calculateFromTree(seqTree, n, ok);

            if(!ok)
                return false;

            seqValues[kPos] << result;
        }

        k += kRange.step;
    }

    return true;
}

bool SeqCalculator::calculateAndSaveFirstValuesTrees()
{
    updateSeqValuesSize();

    if(seqValues[0].size() >= firstValsTrees.size())
        return true;

    bool ok = true;
    double result = 0;
    k = kRange.start;

    int savedKpos = kPos;

    for(kPos = 0; kPos < drawsNum; kPos++)
    {
        for(int i = 0; i < firstValsTrees.size(); i++)
        {
            result = calculateFromTree(firstValsTrees[i], 0, ok);

            if(!ok)
                return false;

            seqValues[kPos] << result;
        }

        k += kRange.step;
    }

    kPos = savedKpos;

    return true;
}

double SeqCalculator::calculateFromTree(FastTree *tree, double x, bool &ok)
{
    if(!ok)
        return nan("");
    if(tree->type == NUMBER )
    {
        return *tree->value;
    }
    else if(tree->type == VAR_N)
    {
        return x;
    }
    else if(tree->type == PAR_K)
    {
        return k;
    }
    else if(tree->type == PLUS)
    {
        return calculateFromTree(tree->left, x, ok) + calculateFromTree(tree->right, x, ok);
    }
    else if(tree->type == MINUS)
    {
        return calculateFromTree(tree->left, x, ok) - calculateFromTree(tree->right, x, ok);
    }
    else if(tree->type == MULTIPLY)
    {
        return calculateFromTree(tree->left, x, ok) * calculateFromTree(tree->right, x, ok);
    }
    else if(tree->type == DIVIDE)
    {
        return calculateFromTree(tree->left, x, ok) / calculateFromTree(tree->right, x, ok);
    }
    else if(tree->type == POW)
    {
        return pow(calculateFromTree(tree->left, x, ok), calculateFromTree(tree->right, x, ok));
    }
    else if(REF_FUNC_START < tree->type && tree->type < REF_FUNC_END)
    {
        return (*refFuncs[tree->type - REF_FUNC_START - 1])(calculateFromTree(tree->right, x, ok));
    }
    else if(FUNC_START < tree->type && tree->type < FUNC_END)
    {
        int id = tree->type - FUNC_START - 1;
        return funcCalculatorsList[id]->getFuncValue(calculateFromTree(tree->right, x, ok), k);
    }
    else if(DERIV_START < tree->type && tree->type < DERIV_END)
    {
        int id = tree->type - DERIV_START - 1;
        return funcCalculatorsList[id]->getDerivativeValue(calculateFromTree(tree->right, x, ok), k);
    }
    else if(tree->type == seqNum + SEQUENCES_START + 1)
    {
        double asked_n = calculateFromTree(tree->right, x, ok);
        ok = verifyAskedTerm(asked_n);
        if(ok)
            return seqValues[kPos][asked_n];
        else return nan("");
    }
    else if(SEQUENCES_START < tree->type && tree->type < SEQUENCES_END)
    {
        int id = tree->type - SEQUENCES_START - 1;
        double asked_n = calculateFromTree(tree->right, x, ok);
        ok = verifyOtherSeqAskedTerm(asked_n, id);
        if(ok)
            return seqCalculatorsList[id]->getCustomSeqValue(asked_n, ok, k);
        else return nan("");
    }

    else return nan("");
}

bool SeqCalculator::verifyAskedTerm(double n)
{
    if(ceil(n) != n || n-nMin >= seqValues[kPos].size())
    {
        errorMessageLabel->setText(tr("Invalid recursion."));

        return false;       
    }
    else if(n < nMin)
    {
        errorMessageLabel->setText(tr("Insufficient number of entered first values."));

        return false;
    }
    else return true;
}

bool SeqCalculator::verifyOtherSeqAskedTerm(double n, int id)
{
    if(ceil(n) != n)
    {
        errorMessageLabel->setText(tr("This sequence calls ") + seqsNames[id] + tr(" with a non-integral value."));

        return false;
    }
    else if(n < nMin)
    {
        errorMessageLabel->setText(tr("This sequence calls ") + seqsNames[id] + tr(" with a value that is lower than n<sub>min</sub>"));

        return false;
    }
    else return true;
}

void SeqCalculator::addRefFuncsPointers()
{
    refFuncs << acos << asin << atan << cos << sin << tan << sqrt
             << log10 << log << fabs << exp << floor << ceil << cosh
             << sinh << tanh << tenPower << tenPower << acosh << asinh
             << atanh << erf << erfc << tgamma << tgamma << cosh
             << sinh << tanh << acosh << asinh << atanh;
}

bool SeqCalculator::check_called_funcs_and_seqs_validity()
{
    isValid = checkCalledFuncsValidity(expression);
    if(!isValid)
    {
        errorMessageLabel->setText(tr("This sequence calls in its expression a function who is either invalid or undefined."));
        //Une fonction invalide ou non définie est appelée dans l'expression de cette suite.
        return false;
    }

    isValid = checkCalledFuncsValidity(firstValsExpr);
    if(!isValid)
    {
        errorMessageLabel->setText(tr("The entered first values use an undefined or an invalid function."));
        //Une fonction invalide ou non définie est appelée dans les premiers termes saisis.
        return false;
    }

    isValid = checkCalledSeqsValidity(expression);
    if(!isValid)
    {
        errorMessageLabel->setText(tr("This sequence uses another sequence who is either invalid or undefined."));
        //Une suite invalide ou non définie est appelée dans l'expression de cette suite
        return false;
    }

    isValid = checkCalledSeqsValidity(firstValsExpr);
    if(!isValid)
    {
        errorMessageLabel->setText(tr("The entered first values use an undefined or an invalid sequence."));
        //Une suite invalide ou non définie est appelée dans les premiers termes saisis.
        return false;
    }

    return true;
}

bool SeqCalculator::checkCalledFuncsValidity(QString str)
{
    QList<int> calledFuncsList = treeCreator.getCalledFuncs(str);

    bool validity = true;

    for(int i = 0; i < calledFuncsList.size() && validity; i++)
        validity = funcCalculatorsList[calledFuncsList[i]]->canBeCalled();

    return validity;
}

bool SeqCalculator::checkCalledSeqsValidity(QString str)
{
    QList<int> calledSeqsList = treeCreator.getCalledSeqs(str);

    bool validity = true;

    for(int i = 0; i < calledSeqsList.size() && validity; i++)
        validity = seqCalculatorsList[calledSeqsList[i]]->canBeCalled();

    return validity;
}

bool SeqCalculator::canBeCalled()
{
    return isExprValidated && areFirstValsValidated;
}

bool SeqCalculator::validateSeqFirstValsTrees()
{
    deleteFirstValsTrees();

    if(firstValsExpr.isEmpty())
        return true;

    firstValsExpr.remove(" ");
    QString str;
    FastTree *tree;

    bool ok = true;

    int count = firstValsExpr.count(';') + 1;

    for(short i = 0; i < count; i++)
    {
        str = firstValsExpr.section(';', i, i);
        tree = treeCreator.getTreeFromExpr(str, ok);

        if(!ok)
            return false;

        firstValsTrees << tree;
    }

    return true;
}

void SeqCalculator::deleteFirstValsTrees()
{
    for(int i = 0; i < firstValsTrees.size() ; i++)
    {
        firstValsTreeCreator.deleteFastTree(firstValsTrees[i]);
    }

    firstValsTrees.clear();
}
