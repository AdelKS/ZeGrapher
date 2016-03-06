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

#include "Calculus/funcvaluessaver.h"

FuncValuesSaver::FuncValuesSaver(QList<FuncCalculator*> funcsList, double pxStep)
{    
    funcs = funcsList;
    setPixelStep(pxStep);

    for(short i = 0 ; i < funcs.size() ; i++)
        funcCurves << QList<QPolygonF>();
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

    double x = 0, k = 0, delta1 = 0, delta2 = 0, delta3 = 0;
    int k_pos = 0, end=0, n=0;


    Range range;

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
            funcCurves[i] << QPolygonF();

            for(x = graphRange.Xmin - unitStep ; x <= graphRange.Xmax + unitStep ; x += unitStep)
            {                
                funcCurves[i][k_pos] <<  QPointF( x*xUnit , - funcs[i]->getFuncValue(x, k) * yUnit);

                n = funcCurves[i][k_pos].size();
                if(n > 1)
                    delta3 = fabs(funcCurves[i][k_pos][n-1].y() - funcCurves[i][k_pos][n-2].y());

                if(n > 2 && delta2 > 2*delta1 && delta2 > 2*delta3)
                    funcCurves[i][k_pos].last().setY(NAN);

                delta1 = delta2;
                delta2 = delta3;

            }

            k += range.step;           
        }
    }
}

void FuncValuesSaver::move(GraphRange range)
{
    graphRange = range;

    double x = 0, k = 0, k_step = 0, delta1 = 0, delta2 = 0, delta3 = 0;
    int k_pos = 0;


    for(short i = 0 ; i < funcs.size(); i++)
    {
        if(!funcs[i]->isFuncValid())
            continue;

        k_step = funcs[i]->getParametricRange().step;
        k = funcs[i]->getParametricRange().start;

        for(k_pos = 0; k_pos < funcCurves[i].size() ; k_pos++)
        {
            x = funcCurves[i][k_pos].first().x()/xUnit - unitStep;


            if(x >= graphRange.Xmin)
            {
                delta2 = fabs(funcCurves[i][k_pos][0].y() - funcCurves[i][k_pos][1].y());
                delta1 = fabs(funcCurves[i][k_pos][1].y() - funcCurves[i][k_pos][2].y());

                while(x >= graphRange.Xmin)
                {
                    funcCurves[i][k_pos].prepend(QPointF(x * xUnit, - funcs[i]->getFuncValue(x, k) * yUnit));

                    x -= unitStep;

                    delta3 = fabs(funcCurves[i][k_pos][0].y() - funcCurves[i][k_pos][1].y());

                    if(delta2 > 2*delta1 && delta2 > 2*delta3)
                        funcCurves[i][k_pos].first().setY(NAN);

                    delta1 = delta2;
                    delta2 = delta3;
                }
            }
            else
            {
                while(x < graphRange.Xmin - unitStep)
                {
                    funcCurves[i][k_pos].removeFirst();
                    x += unitStep;
                }
            }

            x = funcCurves[i][k_pos].last().x()/xUnit + unitStep;

            if(x >= graphRange.Xmin)
            {
                int n = funcCurves[i][k_pos].size();
                delta2 = fabs(funcCurves[i][k_pos][n-1].y() - funcCurves[i][k_pos][n-2].y());
                delta1 = fabs(funcCurves[i][k_pos][n-2].y() - funcCurves[i][k_pos][n-3].y());

                while(x <= graphRange.Xmax)
                {
                    funcCurves[i][k_pos] << QPointF(x * xUnit, - funcs[i]->getFuncValue(x, k)  * yUnit);
                    x += unitStep;

                    delta3 = fabs(funcCurves[i][k_pos][0].y() - funcCurves[i][k_pos][1].y());

                    if(n >= 3 && delta2 > 2*delta1 && delta2 > 2*delta3)
                        funcCurves[i][k_pos].last().setY(NAN);

                    delta1 = delta2;
                    delta2 = delta3;
                }
            }
            else
            {
                while(x > graphRange.Xmax + unitStep)
                {
                    funcCurves[i][k_pos].removeLast();
                    x -= unitStep;
                }
            }



            k += k_step;
        }
    }
}

int FuncValuesSaver::getFuncDrawsNum(int func)
{
    return funcCurves[func].size();
}

QPolygonF FuncValuesSaver::getCurve(int func, int curve)
{
    return funcCurves[func][curve];
}
