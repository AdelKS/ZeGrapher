/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#ifndef FUNCTABLE_H
#define FUNCTABLE_H

#include "abstracttable.h"
#include "informations.h"
#include "Calculus/exprcalculator.h"


class FuncTable : public AbstractTable
{
    Q_OBJECT
public:
    explicit FuncTable(Informations *info);
    void setTableParameters(ValuesTableParameters par);    
    
protected slots:
    void cellEdited(QStandardItem *item);
    void kValueEdited();
    void updateTable();
    void precisionEdited();

protected:
    void fillFromRange();
    void emptyCellsFill();

    void add_x_values();
    void add_y_values();    

    Informations *informations;
    FuncCalculator *func;
    ValuesTableParameters parameters;
    QList<double> xValues, yValues;
    ExprCalculator *exprCalc;
    bool disableCellEdit;
    QPalette validPalette, invalidPalette;
    
};

#endif // FUNCTABLE_H
