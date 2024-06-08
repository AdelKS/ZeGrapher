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


#include "regressionvaluessaver.h"

#include <iostream>

using namespace std;
using namespace zg;

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
    recalculate(Point{xUnit, yUnit}, viewMapper);
}

void RegressionValuesSaver::recalculate(const Point &graphUnits,
                                        [[maybe_unused]] const ZeViewMapper &graphView)
{
    // TODO: use graph view to recalculate
    xUnit = graphUnits.x;
    yUnit = graphUnits.y;
    xUnitStep = pixelStep / xUnit;

    curves.clear();

    if(regression->isPolar())
        calculatePolarRegressionCurve();
    else calculateCartesianRegressionCurve();

}

void RegressionValuesSaver::setPixelStep(double dist)
{
    pixelStep = dist;
    recalculate();
}

QList<QPolygonF> &RegressionValuesSaver::getCurves()
{
    return curves;
}

void RegressionValuesSaver::move(ZeViewMapper newRange)
{
    viewMapper = newRange;

    if(regression->isPolar())
        polarMove();
    else cartesianMove();
}

double RegressionValuesSaver::arg(QPointF pt)
{
    if(pt.y() == 0)
    {
        if(pt.x() >= 0)
            return 0;
        else return -M_PI;
    }

    double res = 2*atan(pt.y() / (pt.x() + length(pt)));

    return res;
}

Range RegressionValuesSaver::getGraphAngleRange()
{
    QRectF graphWin = viewMapper.getGraphRect();
    Range angleRange;

    if(graphWin.contains(0,0))
    {
        angleRange.start = -M_PI;
        angleRange.end = M_PI;
        return angleRange;
    }

    if(graphWin.left() >= 0 && graphWin.bottom() <=0 && graphWin.top() >= 0)
        //correction to the only discontinuity case since arg gives values in [0,2pi] and angle start must always be smaller than end
    {
        angleRange.start = arg(graphWin.bottomLeft());
        angleRange.end = arg(graphWin.topLeft());
    }
    else if(graphWin.left() >= 0 && graphWin.bottom() >=0)
    {
        angleRange.start = arg(graphWin.bottomRight());
        angleRange.end = arg(graphWin.topLeft());
    }
    else if(graphWin.left() <= 0 && graphWin.right() >=0 && graphWin.bottom() >= 0)
    {
        angleRange.start = arg(graphWin.bottomRight());
        angleRange.end = arg(graphWin.bottomLeft());
    }
    else if(graphWin.right() <= 0 && graphWin.bottom() >=0)
    {
        angleRange.start = arg(graphWin.topRight());
        angleRange.end = arg(graphWin.bottomLeft());
    }
    else if(graphWin.right() <= 0 && graphWin.bottom() <=0 && graphWin.top() >= 0)
    {
        angleRange.start = arg(graphWin.topRight());
        angleRange.end = arg(graphWin.bottomRight())+2*M_PI;
    }
    else if(graphWin.right() <= 0 && graphWin.top() <=0)
    {
        angleRange.start = arg(graphWin.topLeft());
        angleRange.end = arg(graphWin.bottomRight());
    }
    else if(graphWin.right() >= 0 && graphWin.left() <=0 && graphWin.top() <= 0)
    {
        angleRange.start = arg(graphWin.topLeft());
        angleRange.end = arg(graphWin.topRight());
    }
    else // if(graphWin.left() >= 0 && graphWin.top() <=0)
    {
        angleRange.start = arg(graphWin.bottomLeft());
        angleRange.end = arg(graphWin.topRight());
    }

    return angleRange;

}

void RegressionValuesSaver::polarMove()
{
    curves.clear();

    calculatePolarRegressionCurve();
}

void RegressionValuesSaver::cartesianMove()
{
    drawRange.start = std::max(viewMapper.x.getMin<view>().v, regression->getDrawRange().start);
    drawRange.end = std::min(viewMapper.x.getMax<view>().v, regression->getDrawRange().end);

    double x = curves.first().first().x() - xUnitStep;

    if(x >= drawRange.start)
    {
        while(x >= drawRange.start)
        {
            curves.first().prepend(QPointF(x ,  regression->eval(x) * yUnit));
            x -= xUnitStep;
        }
    }
    else
    {
        while(x < drawRange.start - xUnitStep)
        {
            curves.first().removeFirst();
            x += xUnitStep;
        }
    }

    x = curves.first().last().x() + xUnitStep;

    if(x >= drawRange.start)
    {
        while(x <= drawRange.end)
        {
            curves.first() << QPointF(x ,  regression->eval(x));
            x += xUnitStep;
        }
    }
    else
    {
        while(x > drawRange.end + xUnitStep)
        {
            curves.first().removeLast();
            x -= xUnitStep;
        }
    }
}

void RegressionValuesSaver::calculateCartesianRegressionCurve()
{
    drawRange.start = std::max(viewMapper.x.getMin<view>().v, regression->getDrawRange().start);
    drawRange.end = std::min(viewMapper.x.getMax<view>().v, regression->getDrawRange().end);

    double x = drawRange.start - xUnitStep;

    QPolygonF curve;
    curve.reserve((int)((drawRange.end - drawRange.start)/xUnitStep));

    while(x <= drawRange.end + xUnitStep)
    {
        curve << QPointF(x,  regression->eval(x));
        x += xUnitStep;
    }

    curves << curve;
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

    // limit the angle's range to graphrange.

    Range regRange = regression->getDrawRange();
    graphAngleRange = getGraphAngleRange();

    double angle = regRange.start, radius = 0, deltaAngle = 0.0001;
    double normalisedAngle;
    //QPointF nextPt, delta;

    QPolygonF curve;
    QPointF pt;
    QRectF graphRect = viewMapper.getGraphRect();

    while(angle < regRange.end)
    {
        normalisedAngle = remainder(angle, 2*M_PI);

        if(graphAngleRange.end > M_PI && normalisedAngle <= remainder(graphAngleRange.end, 2*M_PI))
            normalisedAngle += 2*M_PI;


        if(normalisedAngle < graphAngleRange.start - deltaAngle)
        {
            angle += graphAngleRange.start - normalisedAngle;
            if(curve.size() > 1)
                curves << curve;
            curve.clear();
        }
        else if(graphAngleRange.end + deltaAngle < normalisedAngle)
        {
            angle += 2*M_PI - (graphAngleRange.end - graphAngleRange.start) - (normalisedAngle - graphAngleRange.end);
            if(curve.size() > 1)
                curves << curve;
            curve.clear();
        }
        else
        {
            radius = regression->eval(angle);

            pt.setX(radius * cos(angle));
            pt.setY(radius * sin(angle));

            if(graphRect.contains(pt))
            {
                curve << QPointF(radius * cos(angle), - radius * sin(angle));

                deltaAngle = min(length(pt) + 0.0000001, fabs(atan(pixelStep/sqrt(QPointF::dotProduct(curve.last(), curve.last())))));
                angle += deltaAngle;
            }
            else // we look for the next point who's in the represented range, without making more than 2000 tries
            {
                curves << curve;
                curve.clear();
                double step = (regRange.end - angle)/4096;
                while(angle < regRange.end && !graphRect.contains(pt))
                {
                    angle += step;
                    radius = regression->eval(angle);
                    pt.setX(radius * cos(angle));
                    pt.setY(radius * sin(angle));
                }
            }

            /*
            radius = regression->eval(angle);
            nextPt = QPointF(radius * cos(angle) * xUnit, - radius * sin(angle) * yUnit);

            angle -= basicDeltaAngle;

            delta = nextPt - curve.last();
            delta *= pixelStep/sqrt(QPointF::dotProduct(delta, delta));

            nextPt = curve.last() + delta;

            deltaAngle = max(0.000001, fabs(acos((QPointF::dotProduct(curve.last(), nextPt) / (length(curve.last() * length(nextPt)))))));

            angle += deltaAngle;
            */
        }
    }

    if(curve.size() > 1)
        curves << curve;
    curve.clear();

}


RegressionValuesSaver::~RegressionValuesSaver()
{

}
