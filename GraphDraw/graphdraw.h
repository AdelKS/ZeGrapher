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


#ifndef GRAPHDRAW_H
#define GRAPHDRAW_H

#include <QWidget>
#include "Structures.h"
#include "Calculus/funccalculator.h"
#include "Widgets/tangentwidget.h"
#include "Widgets/straightlinewidget.h"
#include "Widgets/pareqwidget.h"
#include "informations.h"
#include "Calculus/funcvaluessaver.h"
#include "Calculus/regressionvaluessaver.h"


class GraphDraw : public QWidget // Abstract class
{
    Q_OBJECT
public:
    explicit GraphDraw(Informations *info);
    ~GraphDraw();

protected:

    void drawRhombus(QPointF pt,double w); inline
    void drawDisc(QPointF pt, double w); inline
    void drawSquare(QPointF pt, double w); inline
    void drawTriangle(QPointF pt, double w); inline
    void drawCross(QPointF pt, double w); inline

    void drawOneFunction(int id, int width, int curveNum = -1);
    void drawOneSequence(int id, int width);
    void drawDataSet(int id, int width);
    void drawRegression(int reg, int width);
    void drawOneTangent(int id);

    void drawFunctions();
    void drawRegressions();
    void drawData();
    void drawSequences();
    void drawTangents(); //except the exception :) indicated by tangentDrawException
    void drawStraightLines();
    void drawStaticParEq();


    int graphWidth, graphHeight;
    Informations *informations;
    FuncValuesSaver *funcValuesSaver;    
    QPainter painter;
    Options parametres;
    QPolygonF polygon;
    QPen pen;
    QBrush brush;
    Point centre;
    GraphRange graphRange;

    double coef;

    double uniteX, uniteY;
    bool moving, recalculate, recalculateRegressions;
    int tangentDrawException;

    QList<FuncCalculator*> funcs;
    QList<SeqCalculator*> seqs;
    QList<StraightLineWidget*> *straightLines;
    QList<TangentWidget*> *tangents;
    QList<ParEqWidget*> *parEqs;
    QList< QList< QList<double> > > *funcVals;
    QList< QList<double> > *regVals;

    QFont font;
};

#endif // GRAPHDRAW_H
