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

RegressionValuesSaver::RegressionValuesSaver(Informations *info)
{
    informations = info;       
}

void RegressionValuesSaver::recalculate(double new_xUnit, double new_yUnit)
{   
    xUnit = new_xUnit;
    yUnit = new_yUnit;
    pixelStep = informations->getOptions().distanceEntrePoints;
    xUnitStep = pixelStep / xUnit;


    regressionCurves.clear();
    regressionCurves.reserve(informations->getRegressionsCount());

    Regression *reg;

    for(int i = 0; i < informations->getRegressionsCount(); i++)
    {       
        reg = informations->getRegression(i);

        if(reg->isPolar())
            calculatePolarRegressionCurve(reg);
        else calculateCartesianRegressionCurve(reg);
    }
}

QPolygonF& RegressionValuesSaver::getCurve(int reg)
{
    return regressionCurves[reg];
}

void RegressionValuesSaver::calculateCartesianRegressionCurve(Regression *reg)
{
    Range range = reg->getDrawRange();
    double x = range.start;

    QPolygonF curve;
    curve.reserve((int)((range.end - range.start)/xUnitStep));

    while(x <= range.end)
    {
        curve << QPointF(x * xUnit, reg->eval(x) * yUnit);
        x += xUnitStep;
    }

    regressionCurves << curve;
}

void RegressionValuesSaver::calculatePolarRegressionCurve(Regression *reg)
{
    // need to finish implementing this
    regressionCurves << QPolygonF();
}


RegressionValuesSaver::~RegressionValuesSaver()
{

}

