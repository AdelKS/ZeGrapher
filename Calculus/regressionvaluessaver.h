/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
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

#ifndef REGRESSIONVALUESSAVER_H
#define REGRESSIONVALUESSAVER_H

#include <algorithm>

#include "regression.h"
#include "Structures.h"


class RegressionValuesSaver : public QObject
{
    Q_OBJECT

public:
    RegressionValuesSaver(Regression *reg, Options opt, GraphRange range, Point graphUnits);
    ~RegressionValuesSaver();

    void setOptions(Options opt);
    bool getDrawState();
    QColor getColor();

    QPolygonF& getCurve();

public slots:
     void recalculate();
     void recalculate(double new_xUnit, double new_yUnit, GraphRange range);

protected:
    void calculatePolarRegressionCurve();
    void calculateCartesianRegressionCurve();
    inline double squareLength(QPointF pt);
    inline double length(QPointF pt);
    QPointF orthogonalVector(const QPointF &pt);

    Regression *regression;
    double xUnit, yUnit, pixelStep, xUnitStep;
    GraphRange graphRange;
    Options options;
    Range drawnRange;

    QPolygonF curve;
};

#endif // REGRESSIONVALUESSAVER_H
