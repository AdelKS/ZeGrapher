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





#ifndef FUNCVALUESSAVER_H
#define FUNCVALUESSAVER_H

#include "information.h"

class FuncValuesSaver
{
public:
    FuncValuesSaver(QList<FuncCalculator *> funcsList, double pxStep);

    void setPixelStep(double pxStep);
    void calculateAll(double new_xUnit, double new_yUnit, GraphView view);
    void move(GraphView view);
    int getFuncDrawsNum(int func);

    QList<QPolygonF> getCurve(int func, int curve);



protected slots:
    void recalculateFuncColors(int id);

protected:
    void calculateAllFuncColors();
    double evalFunc(int funId, double x, double k);

    Information *information;
    GraphView graphView;
    QList<FuncCalculator*> funcs;

    double xUnit, yUnit, pixelStep, unitStep;

    QList< QList< QList<QPolygonF> > > funcCurves;
    QList< QList<QColor> > funcColors;
};

#endif // FUNCVALUESSAVER_H
