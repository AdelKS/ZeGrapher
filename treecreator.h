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


#ifndef TREECREATOR_H
#define TREECREATOR_H

#include "Structures.h"
#include "calculusdefines.h"

class TreeCreator
{
public:
    TreeCreator(short callingObjectType);

    FastTree* getTreeFromExpr(QString expr, bool &ok);

    QList<int> getCalledFuncs(QString expr);
    QList<int> getCalledSeqs(QString expr);

    void allow_k(bool state);
    void deleteFastTree(FastTree *tree);

protected:
    bool check(QString formula);    
    void insertMultiplySigns(QString &formule);
    void refreshAuthorizedVars();
    FastTree* createFastTree(int debut, int fin);

    short funcType;
    QStringList refFunctions, functions, sequences, antiderivatives, derivatives, vars;
    QList<QChar> operators;
    QList<short> decompPriorites, decompTypes, operatorsPriority, operatorsTypes;
    QList<double> decompValeurs;
    QList<double> authorizedVars;
    QString pi;

};

#endif // TREECREATOR_H
