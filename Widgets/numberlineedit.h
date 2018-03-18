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


#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

#include <QLineEdit>

#include "Calculus/exprcalculator.h"
#include "Calculus/funccalculator.h"

class NumberLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    NumberLineEdit(bool allowK = false, QList<FuncCalculator*> otherFuncs = QList<FuncCalculator*>());
    void setNumber(double num);
    bool isValid();
    double getValue();

public slots:
    void clear();

protected slots:
    void checkVal();

signals:
    void newVal(double val);

protected:
    ExprCalculator calculator;
    bool valid;
    double value;
    QPalette validPalette, invalidPalette, neutralPalette;
};

#endif // NUMBERLINEEDIT_H
