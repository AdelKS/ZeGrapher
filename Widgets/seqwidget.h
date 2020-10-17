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




#ifndef SEQWIDGET_H
#define SEQWIDGET_H

#include "Widgets/abstractfuncwidget.h"
#include "Calculus/seqcalculator.h"
#include "Calculus/colorsaver.h"
#include "Widgets/funcwidget.h"
#include "Widgets/expressionlineedit.h"

class SeqWidget : public AbstractFuncWidget
{
    Q_OBJECT
public:
    explicit SeqWidget(QChar name, int id, QColor color);

    void firstValidation();
    void secondValidation();
    void thirdValidation();
    void setFuncsList(QList<FuncCalculator*> list);
    void setFuncWidgets(QList<FuncWidget*> widgets);
    void setSeqWidgets(QList<SeqWidget*> widgets);

    bool isSeqParametric();

    SeqCalculator* getCalculator();

public slots:
    void checkCalledFuncsSeqsParametric();

protected slots:
    void checkExprLineEdit();
    void checkFirstValsLineEdit();

signals:
    void drawStateChanged();
    void newParametricState();
    
protected:
    void addSeqWidgets();
    void updateParametricState();

    QList<FuncWidget*> funcWidgets;
    QList<SeqWidget*> seqWidgets;
    SeqCalculator *calculator;
    ExpressionLineEdit *firstValsLine;
    ColorSaver colorSaver;
    QChar seqName;
    Range defaultRange;
    bool areFirstValsParametric, isExprParametric, areCalledFuncsSeqsParametric;
    int seqNum;

    
};

#endif // SEQWIDGET_H
