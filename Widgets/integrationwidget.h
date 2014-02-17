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


#ifndef INTEGRATIONWIDGET_H
#define INTEGRATIONWIDGET_H

#include "Structures.h"
#include "Calculus/exprcalculator.h"

class IntegrationWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit IntegrationWidget(int funcNum, QList<FuncCalculator*> funcsList);

    QList<Point> getIntegrationPoints(bool &ok);

public slots:
    void updateWidgetsShownState(QString expr);

protected slots:
    void assignNeutralPalette(QWidget *lineEdit);

signals:
    void returnPressed();

protected:  
    void addWidgetToList(QString name);

    QList<QWidget*> containerWidgetsList;
    QList<QLineEdit*> xList, yList;
    QList<QString> antiderivatives;
    ExprCalculator exprCalc;
    QPalette invalidPalette, validPalette, neutralPalette;
    QSignalMapper *signalMapper;
    QVBoxLayout *centralLayout;

    short Func;

};

#endif // INTEGRATIONWIDGET_H
