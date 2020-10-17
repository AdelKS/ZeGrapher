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




#ifndef VALUESTABLECONF_H
#define VALUESTABLECONF_H

#include "information.h"
#include "Calculus/exprcalculator.h"

class ValuesTableConf : public QWidget
{
    Q_OBJECT
public:
    explicit ValuesTableConf(Information *info, QWidget *parent = 0);
    ~ValuesTableConf();
    
signals:
    void next(ValuesTableParameters parameters);

protected slots:
     void updateNameCombo();
     void apply();

protected:
     bool verifySeqInfo(double start, double step);

     Information *infoClass;   

     QStringList functions, sequences;

     QComboBox *nameCombo, *typeCombo;
     QRadioButton *fromCurrentGraphic, *manualEntry, *predefined;
     QSpinBox *emptyCellsNum, *cellsNum;
     QLineEdit *startValue, *stepValue;

     ExprCalculator *exprCalc;

     QList<ParEqWidget*> *parEqs;
     QList<FuncCalculator*> funcs;
     QList<SeqCalculator*> seqs;

     QList<short> typesNameMap;
};

#endif // VALUESTABLECONF_H
