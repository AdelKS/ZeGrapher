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




#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H

#include "Calculus/funccalculator.h"
#include "Widgets/abstractfuncwidget.h"
#include "Widgets/integrationwidget.h"
#include "Calculus/colorsaver.h"

class FuncWidget : public AbstractFuncWidget
{
    Q_OBJECT
public:
    explicit FuncWidget(QChar name, int id, QColor color);
    ~FuncWidget();

    void firstValidation();
    void secondValidation();
    void setFuncsCalcsList(QList<FuncCalculator*> list);
    void setFuncWidgets(QList<FuncWidget*> widgets);

    bool isFuncParametric();
    bool validateIntegrationPoints();

    FuncCalculator* getCalculator();

public slots:
    void checkCalledFuncsParametric();

protected slots:
    void resetToNeutralState();
    void checkExprLineEdit();

signals:
    void drawStateChanged();
    void newParametricState(int funcNum);

protected:
    void updateParametricState();

    QList<FuncWidget*> funcWidgets;
    FuncCalculator *calculator;
    IntegrationWidget *integrationWidget;
    ColorSaver colorSaver;
    QChar funcName;
    int funcNum;
    bool areCalledFuncsParametric, isExprParametric;


};

#endif // FUNCWIDGET_H
