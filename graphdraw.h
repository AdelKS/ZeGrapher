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
#include "funccalculator.h"
#include "tangentwidget.h"
#include "straightlinewidget.h"
#include "pareqwidget.h"
#include "informations.h"
#include "funcvaluessaver.h"

class GraphDraw : public QWidget // Abstract class
{
    Q_OBJECT
public:
    explicit GraphDraw(Informations *info);
    ~GraphDraw();

protected:
    void drawOneFunction(int id, int width, int curveNum = -1);
    void drawOneSequence(int id, int width);
    void drawOneTangent(int id);

    void drawFunctions();
    void drawSequences();
    void drawTangents(); //except the exception :) designed by tangentDrawException
    void drawStraightLines();
    void drawStaticParEq();


    int graphWidth, graphHeight;
    Informations *informations;
    FuncValuesSaver *funcValuesSaver;
    QPainter painter;
    Options parametres;
    QPolygonF polygon;
    QPen pen;
    Point centre;
    GraphRange graphRange;

    double uniteX, uniteY;
    bool moving, recalculate;
    int tangentDrawException;

    QList<FuncCalculator*> funcs;
    QList<SeqCalculator*> seqs;
    QList<StraightLineWidget*> *straightLines;
    QList<TangentWidget*> *tangents;
    QList<ParEqWidget*> *parEqs;
    QList< QList< QList<double> > > *funcVals;

    QFont font;
};

#endif // GRAPHDRAW_H
