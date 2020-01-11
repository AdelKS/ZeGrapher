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





#include "Calculus/treecreator.h"

TreeCreator::TreeCreator(ObjectType type)
{
    funcType = type;

    refFunctions << "acos" << "asin" << "atan" << "cos" << "sin" << "tan" << "sqrt"
                 << "log" << "ln" << "abs" << "exp" << "floor" << "ceil" << "cosh"
                 << "sinh" << "tanh" << "E" << "e" << "acosh" << "asinh" << "atanh"
                 << "erf" << "erfc" << "gamma" << "Γ" << "ch" << "sh" << "th" << "ach"
                 << "ash" << "ath";
    functions << "f" << "g" << "h" << "p" << "r" << "m";
    antiderivatives << "F" << "G" << "H" << "P" << "R" << "M";
    derivatives << "f'" << "g'" << "h'" << "p'" << "r'" << "m'";
    sequences << "u" << "v" << "l" << "w" << "q" << "z";

    constants << "π" << "pi" << "Pi" << "PI";
    constantsVals << M_PI << M_PI << M_PI << M_PI ;

    vars << "x" << "t" << "n" << "k";
    authorizedVars << false << false << false << false;

    operators << '^' << '*' << '/' << '+' << '-';
    operatorsPriority << POW << OP_HIGH << OP_HIGH << OP_LOW << OP_LOW;
    operatorsTypes << POW << MULTIPLY << DIVIDE << PLUS << MINUS;

    refreshAuthorizedVars();
}

void TreeCreator::refreshAuthorizedVars()
{
    if(funcType == ObjectType::FUNCTION)
    {
        authorizedVars[0] = authorizedVars[3] = true; // x and k
    }
    else if(funcType == ObjectType::SEQUENCE)
    {
        authorizedVars[2] = authorizedVars[3] = true; // k and n
    }
    else if(funcType == ObjectType::PARAMETRIC_EQ)
    {
        authorizedVars[1] = authorizedVars[3] = true; // k and t
    }
    else if(funcType == ObjectType::DATA_TABLE_EXPR)
    {
        authorizedVars[0] = true; // only x, which is the old cell value.
    }
}

FastTree* TreeCreator::getTreeFromExpr(QString expr, bool &ok, QStringList additionnalVars)
{    
    FastTree *tree = nullptr;

    customVars = additionnalVars;

    insertMultiplySigns(expr);
    ok = check(expr);

    if(ok)
        tree = createFastTree(decompTypes.size()-1, 0);

    return tree;
}

void TreeCreator::allow_k(bool state)
{
    authorizedVars[3] = state;
}

void TreeCreator::insertMultiplySigns(QString &formula)
{
    for(int i = 0 ; i < formula.size()-1; i++)
    {
        if((formula[i].isDigit() && formula[i+1].isLetter()) ||
                (formula[i].isLetter() && formula[i+1].isDigit()) ||
                (formula[i].isDigit() && formula[i+1] == '(') ||
                (formula[i] == ')' && formula[i+1] == '(') ||               
                (formula[i] == ')' && (formula[i+1].isDigit() || formula[i+1].isLetter())))
        {
            formula.insert(i+1, QString("*"));
            i++;
        }
        else if(formula[i] == '-' && formula[i+1].isLetter())
        {
            formula.insert(i+1, QString("1*"));
            i++;
        }
    }
}

QList<int> TreeCreator::getCalledFuncs(QString expr)
{
    QList<int> calledFuncs;

    if(expr.isEmpty())
        return calledFuncs;

    int i = 0, letterStart;

    QStringList calledObjects;

    while(i < expr.size())
    {
        if(expr[i].isLetter())
        {
            letterStart = i;

            while(expr[i].isLetter() && i < expr.size())
                i++;

            calledObjects << expr.mid(letterStart, i - letterStart);
        }
        i++;
    }

    for(i = 0 ; i < functions.size() ; i++)
    {
        if((calledObjects.contains(antiderivatives[i]) || calledObjects.contains(functions[i]) || calledObjects.contains(derivatives[i]))
                && !calledFuncs.contains(i))
            calledFuncs << i;
    }

    return calledFuncs;
}

QList<int> TreeCreator::getCalledSeqs(QString expr)
{
    QList<int> calledSeqs;

    if(expr.isEmpty())
        return calledSeqs;

    int i = 0, letterStart;

    QStringList calledObjects;

    while(i < expr.size())
    {
        if(expr[i].isLetter())
        {
            letterStart = i;

            while(expr[i].isLetter() && i < expr.size())
                i++;

            calledObjects << expr.mid(letterStart, i - letterStart);
        }
        i++;
    }

    for(i = 0 ; i < sequences.size() ; i++)
    {
        if(calledObjects.contains(sequences[i]) && !calledSeqs.contains(i))
            calledSeqs << i;
    }

    return calledSeqs;
}

bool TreeCreator::check(QString formula)
{
    formula.remove(' ');
    formula.replace("²", "^2");
    decompPriorites.clear();
    decompTypes.clear();
    decompValues.clear();

    if(formula.isEmpty())
        return false;

    bool digit = true, openingParenthesis = true, numberSign = true, varOrFunc = true, canEnd = false,
         ope = false, closingParenthesis = false;

    short pth = 0;

    for(int i = 0 ; i < formula.size(); i++)
    {
        if((formula[i].isDigit() && digit) || ((formula[i]=='-' || formula[i] == '+') && numberSign && i+1 < formula.size() && formula[i+1].isDigit()))
        {
            bool ok = false, dejavirgule = false;
            int numStart = i, numDigits = 1;
            i++;

            while(!ok && i < formula.size())
            {
                if(formula[i].isDigit())
                    i++;
                else if((formula[i]==',' || formula[i]=='.') && !dejavirgule)
                {
                    dejavirgule = true;
                    i++;
                }
                else ok = true;
            }

            numDigits = i - numStart;
            i--;

            QString number = formula.mid(numStart, numDigits);
            decompPriorites << NUMBER;
            decompTypes << NUMBER;
            decompValues << number.toDouble(&ok);
            if(!ok)
                return false;

            digit = openingParenthesis = varOrFunc = numberSign = false;
            ope = closingParenthesis = canEnd = true;
        }
        else if(formula[i].isLetter() && varOrFunc)
        {
            int letterPosStart = i;

            while(i+1 < formula.size() && (formula[i+1].isLetter() || formula[i+1] == '_')) { i++ ; }

            if(i != formula.size() && formula[i+1] == '\'')
                i++;

            int numLetters = i - letterPosStart + 1;           

            QString name = formula.mid(letterPosStart, numLetters);

            if(refFunctions.contains(name) || antiderivatives.contains(name) || functions.contains(name) ||
                    derivatives.contains(name) || sequences.contains(name))
            {
                if(i+1 >= formula.size() || (formula[i+1] != '(' && formula[i] != 'e' && formula[i] != 'E'))
                    return false;

                decompPriorites << FUNC;
                decompValues << 0.0;
                openingParenthesis = true;
                digit = ope = canEnd = closingParenthesis = varOrFunc = numberSign = false;

                if(refFunctions.contains(name))
                {
                    decompTypes << refFunctions.indexOf(name) + REF_FUNC_START + 1;

                    if(name == "E" || name == "e")
                        digit = numberSign = openingParenthesis = true;
                }

                else if(antiderivatives.contains(name) && funcType == FUNCTION)
                    decompTypes << antiderivatives.indexOf(name) + INTEGRATION_FUNC_START + 1;

                else if(functions.contains(name))
                    decompTypes << functions.indexOf(name) + FUNC_START + 1;

                else if(derivatives.contains(name))
                    decompTypes << derivatives.indexOf(name) + DERIV_START + 1;

                else if(sequences.contains(name) && funcType == SEQUENCE)
                    decompTypes << sequences.indexOf(name) + SEQUENCES_START + 1;

                else return false;
            }

            else if(constants.contains(name) || customVars.contains(name) || vars.contains(name))
            {
                varOrFunc = numberSign = openingParenthesis = digit = false;
                ope = closingParenthesis = canEnd = true;

                if(customVars.contains(name)) /* customVars comes at first because of overriding policy, customvars come from dataplot, and user can redefine
                                                n t or x or k */
                {
                    decompTypes << ADDITIONNAL_VARS_START + customVars.indexOf(name);
                    decompPriorites << VAR;
                    decompValues << customVars.indexOf(name);
                }
                else if(constants.contains(name))
                {
                    decompPriorites << NUMBER;
                    decompTypes << NUMBER;
                    decompValues << constantsVals[constants.indexOf(name)];
                }
                else if(vars.contains(name) && authorizedVars[vars.indexOf(name)])
                {
                    decompTypes << vars.indexOf(name) + VARS_START + 1;
                    decompPriorites << VAR;
                    decompValues << 0.0;
                }                           

                else return false;
            }

            else return false;

        }       
        else if(operators.contains(formula[i]) && ope)
        {
            short pos = operators.indexOf(formula[i]);

            decompTypes << operatorsTypes[pos];
            decompPriorites << operatorsPriority[pos];
            decompValues << 0.0 ;

            openingParenthesis = digit = varOrFunc = true;
            ope = numberSign = closingParenthesis = canEnd = false;
        }
        else if(formula[i]=='(' && openingParenthesis)
        {           
            pth++;

            decompTypes << PTHO ;
            decompPriorites << PTHO;
            decompValues << 0.0 ;

            numberSign = digit = varOrFunc = openingParenthesis = true;
            ope = closingParenthesis = canEnd = false;
        }
        else if(formula[i]==')' && closingParenthesis && pth > 0)
        {            
            pth--;

            decompTypes << PTHF ;
            decompPriorites << PTHF ;
            decompValues << 0.0 ;

            ope = closingParenthesis = canEnd = true;
            digit = numberSign = openingParenthesis = varOrFunc = false;

        }        
        else return false;
    }

    return pth == 0 && canEnd;
}

FastTree* TreeCreator::createFastTree(int debut, int fin)
{
    FastTree *root = new FastTree;
    root->right = nullptr;
    root->left = nullptr;
    root->value = nullptr;

    short pths = 0, closingPthPos = 0, openingPthPos = 0;
    bool debutPthFerme = false;

    if(debut == fin)
    {
        if(decompPriorites[debut] == NUMBER)
        {
            root->type = NUMBER;
            root->value = new double;
            *root->value = decompValues[debut];
        }
        else root->type = decompTypes[debut];

        return root;
    }

    for(char op = 0; op < 5; op++)
    {
        for(short i = debut ; i >= fin ; i--)
        {
            if(decompPriorites[i] == PTHF)
            {
                if(!debutPthFerme)
                {
                    debutPthFerme = true;
                    closingPthPos = i - 1;
                }
                pths--;
            }
            else if(decompPriorites[i] == PTHO)
            {
                pths++;
                if(pths == 0)
                {
                    openingPthPos = i + 1;
                    if(op == PTHO)
                    {
                        delete root;
                        root = createFastTree(closingPthPos, openingPthPos);
                        return root;
                    }
                }
            }
            else if(pths == 0 && decompPriorites[i] == op)
            {
                root->type = decompTypes[i];
                root->right = createFastTree(debut, i + 1);
                if(op != FUNC)
                    root->left = createFastTree(i - 1, fin);
                return root;
            }
        }
    }
    return root;
}

void TreeCreator::deleteFastTree(FastTree *tree)
{
    delete tree->value;
    if(tree->left != nullptr)
        deleteFastTree(tree->left);
    if(tree->right != nullptr)
        deleteFastTree(tree->right);
    delete tree;
}
