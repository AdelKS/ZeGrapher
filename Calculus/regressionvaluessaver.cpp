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
    graphRange = info->getRange();
    deplacement = 0;
}

void RegressionValuesSaver::calculateAll(double new_xUnit, double new_yUnit)
{
    graphRange = informations->getRange();
    xUnit = new_xUnit;
    yUnit = new_yUnit;
    pixelStep = informations->getOptions().distanceEntrePoints;
    unitStep = pixelStep / xUnit;

    double x = 0;

    deplacement = 0;
    startAbscissa_unit = trunc(graphRange.Xmin / unitStep) * unitStep - unitStep;
    startAbscissa_pixel = startAbscissa_unit * xUnit;

    endAbscissa_unit = trunc(graphRange.Xmax / unitStep) * unitStep + unitStep;
    endAbscissa_pixel = endAbscissa_unit * xUnit;

    regressions = informations->getRegressionsList();
    regressionVals.clear();
    regressionVals.reserve(regressions.size());

    for(short i = 0; i < regressions.size(); i++)
    {
        QList<double> list;

        for(x = startAbscissa_unit ; x <= endAbscissa_unit ; x += unitStep)
        {
            list << regressions.at(i)->eval(x);
        }

        regressions << list;
    }
}

void RegressionValuesSaver::move(double pixels)
{
    double x = 0, x_start = 0, x_step = 0;

    deplacement += pixels;
    double limite = trunc(deplacement / pixelStep);

    if(limite == 0)
        return;

    if(deplacement < 0)
    {
        x_step = unitStep;
        x_start = endAbscissa_unit;
    }
    else
    {
        x_step = - unitStep;
        x_start = startAbscissa_unit;
    }

    for(short i = 0 ; i < regressions.size(); i++)
    {
        x = x_start;

        for(double j = 0; j < abs(limite); j++)
        {
            x += x_step;

            if(deplacement < 0)
            {
                regressionVals[i].append(regressions[i]->eval(x));
                regressionVals[i].removeFirst();
            }
            else
            {
                regressionVals[i].prepend(regressions[i]->eval(x));
                regressionVals[i].removeLast();
            }

        }
    }

    deplacement -= pixelStep * limite;
    startAbscissa_pixel -= pixelStep * limite;
    endAbscissa_unit -= limite * unitStep;
    startAbscissa_unit -= limite * unitStep;
}

double RegressionValuesSaver::getStartAbsicssaUnit()
{
    return startAbscissa_unit;
}

double RegressionValuesSaver::getStartAbscissaPixel()
{
    return startAbscissa_pixel;
}

QList< QList<double> >* RegressionValuesSaver::getRegressionValsListPointer()
{
    return &regressionVals;
}

RegressionValuesSaver::~RegressionValuesSaver()
{

}

