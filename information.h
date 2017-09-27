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


#ifndef INFORMATION_H
#define INFORMATION_H

#include "structures.h"
#include "Widgets/pareqwidget.h"
#include "Calculus/seqcalculator.h"
#include "Calculus/funccalculator.h"
#include "Widgets/straightlinewidget.h"
#include "Widgets/tangentwidget.h"
#include "Calculus/colorsaver.h"
#include "Calculus/regressionvaluessaver.h"

class Information: public QObject
{
    Q_OBJECT

public:
    Information();

    ZeGraphView getGraphView();
    GraphSettings getGraphSettings();

    void addDataList();
    void removeDataList(int index);
    void setData(int index, QList<QPointF> list);
    void setDataStyle(int index, DataStyle style);

    int getDataListsCount();
    QList<QPointF> getDataList(int index);
    DataStyle getDataStyle(int index);

    void addDataRegression(Regression *reg);
    void removeDataRegression(Regression *reg);
    Regression* getRegression(int index);
    QList<Regression*> getRegressions();
    int getRegressionsCount();

    void setParEqsListPointer(QList<ParEqWidget*> *list);
    QList<ParEqWidget*>* getParEqsList();

    void setTangentsListPointer(QList<TangentWidget*> *list);
    QList<TangentWidget*>* getTangentsList();

    void setStraightLinesListPointer(QList<StraightLineWidget*> *list);
    QList<StraightLineWidget*>* getStraightLinesList(); 

    void checkParametricEquations();

    void setSequencesList(QList<SeqCalculator*> list);
    QList<SeqCalculator*> getSeqsList();

    void setFunctionsList(QList<FuncCalculator*> list);
    QList<FuncCalculator*> getFuncsList();

    void setUnits(Point vec);
    Point getUnits();

    bool isOrthonormal();

public slots:
    void emitUpdateSignal();
    void emitDataUpdate();
    void emitDrawStateUpdate();
    void emitAnimationUpdate();

signals:

    void dataUpdated();
    void gridStateChange();
    void updateOccured();
    void drawStateUpdateOccured();
    void animationUpdate();
    void regressionAdded(Regression *reg);
    void regressionRemoved(Regression *reg);
    void newGraphSettings();

public slots:

    void setRange(const ZeGraphView &newWindow);
    void changeGridState();
    void setOrthonormal(bool state);
    void setGraphSettings(GraphSettings opt);

protected:

    QList<QList<QPointF> > data;
    QList<DataStyle> dataStyle;

    QList<Regression*> regressions;

    QList<TangentWidget*> *tangents;
    QList<StraightLineWidget*> *lines;

    QList<FuncCalculator*> functions;
    QList<SeqCalculator*> sequences;

    ZeGraphView graphView;
    GraphSettings graphSettings;
    bool updatingLock;
    Point units;
    QList<ParEqWidget*> *parEqWidgets;
};

#endif // INFORMATION_H
