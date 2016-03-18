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


#include "Calculus/funcvaluessaver.h"

#include <iostream>

FuncValuesSaver::FuncValuesSaver(QList<FuncCalculator*> funcsList, double pxStep)
{    
    funcs = funcsList;
    setPixelStep(pxStep);

    for(short i = 0 ; i < funcs.size() ; i++)
        funcCurves << QList<QList<QPolygonF>>();
}

void FuncValuesSaver::setPixelStep(double pxStep)
{
    pixelStep = pxStep;
}

void FuncValuesSaver::calculateAll(double new_xUnit, double new_yUnit, GraphRange gRange)
{
    graphRange = gRange;
    xUnit = new_xUnit;
    yUnit = new_yUnit;
    unitStep = pixelStep / xUnit;

    double x = 0, k = 0, delta1 = 0, delta2 = 0, delta3 = 0, y=0;
    int k_pos = 0, end=0, n=0;


    Range range;
    QPolygonF curvePart;
    QPointF pt1, pt2;

    for(short i = 0; i < funcs.size(); i++)
    {
        if(!funcs[i]->isFuncValid())
            continue;

        funcCurves[i].clear();

        range = funcs[i]->getParametricRange();
        end = trunc((range.end - range.start)/range.step) + 1;
        k = range.start;

        for(k_pos = 0 ; k_pos < end && k_pos < PAR_DRAW_LIMIT ; k_pos++)
        {            
            funcCurves[i] << QList<QPolygonF>();
            curvePart.clear();

            for(x = graphRange.Xmin - unitStep ; x <= graphRange.Xmax + unitStep ; x += unitStep)
            {                
                y = funcs[i]->getFuncValue(x, k);

                if(isnan(y) || isinf(y))
                {
                    if(!curvePart.isEmpty())
                    {
                        funcCurves[i][k_pos] << curvePart;
                        curvePart.clear();
                    }
                }
                else
                {
                    curvePart <<  QPointF( x*xUnit , - y * yUnit);

                    n = curvePart.size();
                    if(n > 1)
                        delta3 = fabs(curvePart[n-1].y() - curvePart[n-2].y());

                    if(n > 2 && delta2 > 4*delta1 && delta2 > 4*delta3)
                    {
                        pt1 = curvePart[n-2];
                        pt2 = curvePart[n-1];

                        curvePart.removeLast();
                        curvePart.removeLast();

                        funcCurves[i][k_pos] << curvePart;
                        curvePart.clear();
                        curvePart << pt1 << pt2;
                    }

                    delta1 = delta2;
                    delta2 = delta3;

                }
            }

            if(!curvePart.isEmpty())
                funcCurves[i][k_pos] << curvePart;
            curvePart.clear();

            k += range.step;           
        }
    }
}

void FuncValuesSaver::move(GraphRange range)
{
    graphRange = range;

    double x = 0, k = 0, k_step = 0, delta1 = 0, delta2 = 0, delta3 = 0, y=0;
    int k_pos = 0;


    QPolygonF curvePart;
    QPointF pt1, pt2;
    int n = 0;

    for(short i = 0 ; i < funcs.size(); i++)
    {
        if(!funcs[i]->isFuncValid())
            continue;

        k_step = funcs[i]->getParametricRange().step;
        k = funcs[i]->getParametricRange().start;

        for(k_pos = 0; k_pos < funcCurves[i].size() ; k_pos++)
        {
            std::cout << "curve parts number: " << funcCurves[i][k_pos].size() << std::endl;

            curvePart = funcCurves[i][k_pos].takeFirst();
            x = curvePart.first().x()/xUnit - unitStep;

            if(x >= graphRange.Xmin)
            {
                if(curvePart.size() >= 3)
                {
                    delta2 = fabs(curvePart[0].y() - curvePart[1].y());
                    delta1 = fabs(curvePart[1].y() - curvePart[2].y());
                }
                else if(curvePart.size() == 2)
                {
                    delta2 = fabs(curvePart[0].y() - curvePart[1].y());
                }


                while(x >= graphRange.Xmin)
                {
                    y = funcs[i]->getFuncValue(x, k);

                    if(isnan(y) || isinf(y))
                    {
                        if(!curvePart.isEmpty())
                        {
                            funcCurves[i][k_pos].prepend(curvePart);
                            curvePart.clear();
                        }
                    }
                    else
                    {
                        curvePart.prepend(QPointF(x * xUnit, - y * yUnit));

                        n = curvePart.size();

                        if(n > 1)
                            delta3 = fabs(curvePart[0].y() - curvePart[1].y());

                        if(n > 2 && delta2 > 4*delta1 && delta2 > 4*delta3)
                        {
                            pt1 = curvePart.takeFirst();
                            pt2 = curvePart.takeFirst();

                            funcCurves[i][k_pos].prepend(curvePart);
                            curvePart.clear();
                            curvePart << pt1 << pt2;
                        }

                        delta1 = delta2;
                        delta2 = delta3;

                    }

                    x -= unitStep;
                }
            }
            else
            {
                while(x < graphRange.Xmin - unitStep)
                {
                    if(curvePart.isEmpty())
                        curvePart = funcCurves[i][k_pos].takeFirst();
                    curvePart.removeFirst();
                    x += unitStep;
                }                                
            }

            if(!curvePart.isEmpty())
                funcCurves[i][k_pos].prepend(curvePart);

            curvePart.clear();

            curvePart = funcCurves[i][k_pos].takeLast();

            x = curvePart.last().x()/xUnit + unitStep;

            if(x <= graphRange.Xmax)
            {
                n = curvePart.size();
                if(curvePart.size() >= 3)
                {
                    delta2 = fabs(curvePart[n-1].y() - curvePart[n-2].y());
                    delta1 = fabs(curvePart[n-3].y() - curvePart[n-3].y());
                }
                else if(curvePart.size() == 2)
                {
                    delta2 = fabs(curvePart[n-1].y() - curvePart[n-2].y());
                }

                while(x <= graphRange.Xmax)
                {
                    y = funcs[i]->getFuncValue(x, k);

                    if(isnan(y) || isinf(y))
                    {
                        if(!curvePart.isEmpty())
                        {
                            funcCurves[i][k_pos] << curvePart;
                            curvePart.clear();
                        }
                    }
                    else
                    {
                        curvePart << QPointF(x * xUnit, - y  * yUnit);
                        n = curvePart.size();

                        if(n > 1)
                            delta3 = fabs(curvePart[n-1].y() - curvePart[n-2].y());

                        if(n > 2 && delta2 > 4*delta1 && delta2 > 4*delta3)
                        {
                            pt2 = curvePart.takeLast();
                            pt1 = curvePart.takeLast();

                            funcCurves[i][k_pos] << curvePart;
                            curvePart.clear();
                            curvePart << pt1 << pt2;
                        }

                        delta1 = delta2;
                        delta2 = delta3;
                    }

                    x += unitStep;
                }
            }
            else
            {
                while(x > graphRange.Xmax + unitStep)
                {
                    if(curvePart.isEmpty())
                        curvePart = funcCurves[i][k_pos].takeLast();
                    curvePart.removeLast();
                    x -= unitStep;
                }
            }

            if(!curvePart.isEmpty())
                funcCurves[i][k_pos] << curvePart;

            k += k_step;
        }
    }
}

int FuncValuesSaver::getFuncDrawsNum(int func)
{
    return funcCurves[func].size();
}

QList<QPolygonF> FuncValuesSaver::getCurve(int func, int curve)
{
    return funcCurves[func][curve];
}
