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

RegressionValuesSaver::RegressionValuesSaver(double pixStep, Regression *reg)
{    
    regression = reg;
    pixelStep = pixStep;   
}

RegressionValuesSaver::RegressionValuesSaver(const RegressionValuesSaver &other) : QObject()
{
    regression = other.regression;
    pixelStep = other.pixelStep;
}

RegressionValuesSaver& RegressionValuesSaver::operator=(const RegressionValuesSaver &other)
{
    regression = other.regression;
    pixelStep = other.pixelStep;

    return *this;
}

Regression* RegressionValuesSaver::getRegression()
{
    return regression;
}

void RegressionValuesSaver::setRegression(Regression *reg)
{
    regression = reg;
}

void RegressionValuesSaver::recalculate()
{
    recalculate(Point{xUnit, yUnit}, graphRange);
}

void RegressionValuesSaver::recalculate(Point graphUnits, GraphRange range)
{   
    graphRange = range;
    xUnit = graphUnits.x;
    yUnit = graphUnits.y;
    xUnitStep = pixelStep / xUnit;

    curve.clear();

    if(regression->isPolar())
        calculatePolarRegressionCurve();
    else calculateCartesianRegressionCurve();

}

void RegressionValuesSaver::setPixelStep(double dist)
{
    pixelStep = dist;
    recalculate();
}

QPolygonF &RegressionValuesSaver::getCurve()
{
    return curve;
}

void RegressionValuesSaver::move(GraphRange newRange)
{
    if(regression->isPolar())
        return; //nothing to do if polar: All the points have been calculated correctly previously


    graphRange = newRange;

    drawnRange.start = std::max(graphRange.Xmin, regression->getDrawRange().start);
    drawnRange.end = std::min(graphRange.Xmax, regression->getDrawRange().end);

    double x = curve.first().x()/xUnit - xUnitStep;

    if(x >= drawnRange.start)
    {
        while(x >= drawnRange.start)
        {
            curve.prepend(QPointF(x * xUnit, - regression->eval(x) * yUnit));
            x -= xUnitStep;
        }
    }
    else
    {
        while(x < drawnRange.start - xUnitStep)
        {
            curve.removeFirst();
            x += xUnitStep;
        }
    }

    x = curve.last().x()/xUnit + xUnitStep;

    if(x >= drawnRange.start)
    {
        while(x <= drawnRange.end)
        {
            curve << QPointF(x * xUnit, - regression->eval(x) * yUnit);
            x += xUnitStep;
        }
    }
    else
    {
        while(x > drawnRange.end + xUnitStep)
        {
            curve.removeLast();
            x -= xUnitStep;
        }
    }
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
    return QPointF::dotProduct(pt, pt);
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
    double angle = range.start, radius = 0, deltaAngle, basicDeltaAngle;
    QPointF nextPt, delta;

    curve.clear();

    while(angle < range.end)
    {
        radius = regression->eval(angle);
        curve << QPointF(radius * cos(angle) * xUnit, - radius * sin(angle) * yUnit);

        basicDeltaAngle = min(0.01, fabs(atan(pixelStep/sqrt(QPointF::dotProduct(curve.last(), curve.last())))));
        angle += basicDeltaAngle;

        radius = regression->eval(angle);
        nextPt = QPointF(radius * cos(angle) * xUnit, - radius * sin(angle) * yUnit);

        delta = nextPt - curve.last();
        delta *= pixelStep/sqrt(QPointF::dotProduct(delta, delta));

        nextPt = curve.last() + delta;

        //deltaAngle by al-kashi formula plus we take in count that the xScale and yScale are different

        deltaAngle = fabs(acos((QPointF::dotProduct(curve.last(), nextPt) / (length(curve.last() * length(nextPt))))));

        angle += deltaAngle - basicDeltaAngle;
    }
}


RegressionValuesSaver::~RegressionValuesSaver()
{

}

