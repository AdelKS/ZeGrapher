/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#ifndef REGRESSIONVALUESSAVER_H
#define REGRESSIONVALUESSAVER_H

#include <algorithm>

#include "regression.h"
#include "Structures.h"


class RegressionValuesSaver : public QObject
{
    Q_OBJECT

public:
    RegressionValuesSaver(double pixStep, Regression *reg = nullptr);
    RegressionValuesSaver(const RegressionValuesSaver &other);
    ~RegressionValuesSaver();

    RegressionValuesSaver& operator=(const RegressionValuesSaver &other);

    void setPixelStep(double distBetweenPts);

    Regression* getRegression();
    void setRegression(Regression *reg);
    void move(GraphRange newRange);

    QList<QPolygonF> &getCurves();

public slots:
     void recalculate();
     void recalculate(Point graphUnits, GraphRange range);

protected:
    void calculatePolarRegressionCurve();
    void calculateCartesianRegressionCurve();
    inline double squareLength(QPointF pt);
    inline double length(QPointF pt);
    QPointF orthogonalVector(const QPointF &pt);
    void cartesianMove();
    void polarMove();
    Range getGraphAngleRange();
    QList<Range> getDrawableSet();
    double arg(QPointF pt);

    double pixelMove;
    Regression *regression;
    double xUnit, yUnit, pixelStep, xUnitStep;
    GraphRange graphRange;    
    Range drawRange, graphAngleRange;

    QList<QPolygonF> curves;
};

#endif // REGRESSIONVALUESSAVER_H
