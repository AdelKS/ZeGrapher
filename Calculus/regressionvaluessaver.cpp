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




#include "regressionvaluessaver.h"

#include <iostream>

using namespace std;

RegressionValuesSaver::RegressionValuesSaver(Regression *reg, Options opt, GraphRange range, Point graphUnits)
{
    xUnit = graphUnits.x;
    yUnit = graphUnits.y;
    regression = reg;
    options = opt;
    graphRange = range;
}

void RegressionValuesSaver::recalculate()
{
    recalculate(xUnit, yUnit, graphRange);
}

void RegressionValuesSaver::recalculate(double new_xUnit, double new_yUnit, GraphRange range)
{   
    graphRange = range;
    xUnit = new_xUnit;
    yUnit = new_yUnit;
    pixelStep = options.distanceBetweenPoints;
    xUnitStep = pixelStep / xUnit;

    curve.clear();

    if(regression->isPolar())
        calculatePolarRegressionCurve();
    else calculateCartesianRegressionCurve();

}

bool RegressionValuesSaver::getDrawState()
{
    return regression->getDrawState();
}

QColor RegressionValuesSaver::getColor()
{
    return regression->getColor();
}

void RegressionValuesSaver::setOptions(Options opt)
{
    options = opt;
    recalculate();
}

QPolygonF& RegressionValuesSaver::getCurve()
{
    return curve;
}

void RegressionValuesSaver::calculateCartesianRegressionCurve()
{    
    drawnRange.start = std::max(graphRange.Xmin, regression->getDrawRange().start);
    drawnRange.end = std::min(graphRange.Xmax, regression->getDrawRange().end);

    double x = drawnRange.start - xUnitStep;

    curve.clear();
    curve.reserve((int)((drawnRange.end - drawnRange.start)/xUnitStep));

    while(x <= drawnRange.end + xUnitStep)
    {
        curve << QPointF(x * xUnit, - regression->eval(x) * yUnit);
        x += xUnitStep;
    }
}

double RegressionValuesSaver::squareLength(QPointF pt)
{
    return pt.x()*pt.x() + pt.y()*pt.y();
}

double RegressionValuesSaver::length(QPointF pt)
{
    return sqrt(squareLength(pt));
}

QPointF RegressionValuesSaver::orthogonalVector(const QPointF &pt)
{
    return QPointF(pt.y(), -pt.x());
}

void RegressionValuesSaver::calculatePolarRegressionCurve()
{
    Range range = regression->getDrawRange();
    double angle = range.start, radius = 0, deltaAngle;
    QPointF nextPt, delta;

    curve.clear();

    while(angle < range.end)
    {
        radius = regression->eval(angle);
        curve << QPointF(radius * cos(angle) * xUnit, - radius * sin(angle) * yUnit);

        //we evaluate now de step to add to angle to make the next point "pixelStep" farther than this one on the screen

        delta = orthogonalVector(curve.last());
        delta *= pixelStep / length(delta);

        nextPt = curve.last() + delta;

        //deltaAngle by al-kashi formula plus we take in count that the xScale and yScake are different

        deltaAngle = fabs(acos(( squareLength(curve.last()) + squareLength(nextPt) - pixelStep*pixelStep ) / ( 2 * length(curve.last()) * length(nextPt) )));

        angle += deltaAngle;
    }
}


RegressionValuesSaver::~RegressionValuesSaver()
{

}

