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

#ifndef TREECREATOR_H
#define TREECREATOR_H

#include "structures.h"
#include "calculusdefines.h"

/* TODO: update approach as the following:
    - Parse: aka cut each atom in a formula
    - Evaluate type of atom: separator, number
    - Treat number in a special as to make 1.2E+33 as one atom
    - Check for validity
    - Enable setting custom names for functions and variables
    - Performance improvement: flatten trees
*/

enum ObjectType {FUNCTION, SEQUENCE, PARAMETRIC_EQ, NORMAL_EXPR, DATA_TABLE_EXPR};

class TreeCreator
{
public:
    TreeCreator(ObjectType type);

    FastTree* getTreeFromExpr(QString expr, bool &ok, QStringList additionnalVars = QStringList());

    QList<int> getCalledFuncs(QString expr);
    QList<int> getCalledSeqs(QString expr);

    void allow_k(bool state);
    void deleteFastTree(FastTree *tree);

protected:
    bool check(QString formula);
    void insertMultiplySigns(QString &formula);
    void refreshAuthorizedVars();
    FastTree* createFastTree(int debut, int fin);

    ObjectType funcType;
    static const QStringList refFunctions;
    QStringList functions, sequences, antiderivatives, derivatives, constants, vars, customVars;
    QList<double> constantsVals;

    QList<QChar> operators;
    std::vector<ushort> decompPriorites, decompTypes, operatorsPriority, operatorsTypes;
    std::vector<double> decompValues;
    std::vector<bool> authorizedVars;
    QString pi;

};

#endif // TREECREATOR_H
