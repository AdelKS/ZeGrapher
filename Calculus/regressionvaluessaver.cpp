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

#include "regressionvaluessaver.h"

RegressionValuesSaver::RegressionValuesSaver(Regression *reg, Options opt, GraphRange range)
{
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
    pixelStep = options.distanceEntrePoints;
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

void RegressionValuesSaver::calculatePolarRegressionCurve()
{
    // need to finish implementing this
    curve.clear();
}


RegressionValuesSaver::~RegressionValuesSaver()
{

}

