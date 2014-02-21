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


#include "Calculus/funcvaluessaver.h"

FuncValuesSaver::FuncValuesSaver(Informations *info)
{
    informations = info;
    funcs = info->getFuncsList();
    info->getRange();
    deplacement = 0;

    for(short i = 0 ; i < funcs.size() ; i++)
        funcVals << QList<QList<double> >();
}

void FuncValuesSaver::calculateAll(double new_xUnit, double new_yUnit)
{
    graphRange = informations->getRange();
    xUnit = new_xUnit;
    yUnit = new_yUnit;
    pixelStep = informations->getOptions().distanceEntrePoints;
    unitStep = pixelStep / xUnit;

    double x = 0, k = 0;
    int k_pos = 0, end;

    deplacement = 0;
    startAbscissa_unit = trunc(graphRange.Xmin / unitStep) * unitStep - unitStep;
    startAbscissa_pixel = startAbscissa_unit * xUnit;

    endAbscissa_unit = trunc(graphRange.Xmax / unitStep) * unitStep + unitStep;
    endAbscissa_pixel = endAbscissa_unit * xUnit;

    Range range;

    for(short i = 0; i < funcs.size(); i++)
    {
        if(!funcs[i]->isFuncValid())
            continue;

        funcVals[i].clear();

        range = funcs[i]->getParametricRange();
        end = trunc((range.end - range.start)/range.step) + 1;
        k = range.start;

        for(k_pos = 0 ; k_pos < end ; k_pos++)
        {            
            funcVals[i] << QList<double>();

            for(x = startAbscissa_unit ; x <= endAbscissa_unit ; x += unitStep)
            {
                funcVals[i][k_pos] <<  funcs[i]->getFuncValue(x, k);
            }

            k += range.step;           
        }
    }
}

void FuncValuesSaver::move(double pixels)
{
    double x = 0, k = 0, k_step = 0, x_start = 0, x_step = 0;
    int k_pos;

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

    for(short i = 0 ; i < funcs.size(); i++)
    {
        if(!funcs[i]->getDrawState())
            continue;

        k_step = funcs[i]->getParametricRange().step;
        k = funcs[i]->getParametricRange().start;

        for(k_pos = 0; k_pos < funcVals[i].size() ; k_pos++)
        {
            x = x_start;

            for(double j = 0; j < abs(limite); j++)
            {
                x += x_step;

                if(deplacement < 0)
                {
                    funcVals[i][k_pos].append(funcs[i]->getFuncValue(x, k));
                    funcVals[i][k_pos].removeFirst();
                }
                else
                {
                    funcVals[i][k_pos].prepend(funcs[i]->getFuncValue(x, k));
                    funcVals[i][k_pos].removeLast();
                }

            }

            k += k_step;
        }
    }

    deplacement -= pixelStep * limite;
    startAbscissa_pixel -= pixelStep * limite;
    endAbscissa_unit -= limite * unitStep;
    startAbscissa_unit -= limite * unitStep;
}

double FuncValuesSaver::getStartAbsicssaUnit()
{
    return startAbscissa_unit;
}

double FuncValuesSaver::getStartAbscissaPixel()
{
    return startAbscissa_pixel;
}

QList<QList<QList<double> > > *FuncValuesSaver::getFuncValsListPointer()
{
    return &funcVals;
}
