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



#ifndef SEQTABLE_H
#define SEQTABLE_H

#include "ValuesTable/abstracttable.h"
#include "information.h"
#include "Calculus/seqcalculator.h"
#include "Calculus/exprcalculator.h"

class SeqTable : public AbstractTable
{
    Q_OBJECT
public:
    explicit SeqTable(Information *info);
    void setTableParameters(ValuesTableParameters par);

protected slots:
    void cellEdited(QStandardItem *item);
    void kValueEdited();
    void updateTable();
    void precisionEdited();

protected:
    void fillFromRange();
    void emptyCellsFill();

    void addXValues();
    void addYValues();
    void add_parSeq_y_values();

    ExprCalculator exprCalculator;
    Information *information;
    SeqCalculator *seq;
    ValuesTableParameters parameters;
    QList<double> xValues, yValues;
    bool disableCellEdit;
    QPalette validPalette, invalidPalette;
};

#endif // SEQTABLE_H
