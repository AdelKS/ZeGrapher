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



#include "GraphDraw/graphdraw.h"
#include <iostream>

using namespace std;

GraphDraw::GraphDraw(Information *info)
{
    information = info;

    coef = sqrt(3)/2;

    parameters = info->getOptions();
    graphRange = info->getRange();

    pen.setCapStyle(Qt::RoundCap);
    brush.setStyle(Qt::SolidPattern);

    straightLines = info->getStraightLinesList();
    tangents = info->getTangentsList();
    parEqs = info->getParEqsList();
    funcs = info->getFuncsList();
    seqs = info->getSeqsList();

    moving = false;
    tangentDrawException = -1;

    funcValuesSaver = new FuncValuesSaver(info);
    funcVals = funcValuesSaver->getFuncValsListPointer();
}

void GraphDraw::drawRhombus(QPointF pt, double w)
{   
    QPolygonF polygon;
    polygon << pt + QPointF(-w,0) << pt + QPointF(0,w) << pt + QPointF(w,0) << pt + QPointF(0,-w);

    painter.drawPolygon(polygon);
}

void GraphDraw::drawDisc(QPointF pt, double w)
{
    painter.drawEllipse(pt, w, w);
}

void GraphDraw::drawSquare(QPointF pt, double w)
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    QRectF rect;
    rect.setTopLeft(pt + QPointF(-w,-w));
    rect.setBottomRight(pt + QPointF(w,w));

    painter.drawRect(rect);
}

void GraphDraw::drawTriangle(QPointF pt, double w)
{
    w*=2;
    QPolygonF polygon;
    double d  = w*coef;
    double b = w/2;

    polygon << pt + QPointF(0, -w) << pt + QPointF(d, b) << pt + QPointF(-d,b);

    painter.drawPolygon(polygon);
}

void GraphDraw::drawCross(QPointF pt, double w)
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    pen.setWidth(w);

    painter.drawLine(pt+QPointF(0,2*w), pt+QPointF(0, -2*w));
    painter.drawLine(pt+QPointF(-2*w, 0), pt+QPointF(2*w, 0));
}

void GraphDraw::drawDataSet(int id, int width)
{
    QList<QPointF> list = information->getDataList(id);
    DataStyle style = information->getDataStyle(id);

    for(int i = 0 ; i < list.size(); i++)
    {
        list[i].rx() *= uniteX;
        list[i].ry() *= - uniteY;
    }

    pen.setColor(style.color);
    painter.setPen(pen);

    if(style.drawLines)
    {
        QPolygonF polygon;
        pen.setStyle(style.lineStyle);
        painter.setPen(pen);
        painter.drawPolyline(polygon.fromList(list));
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
    }

    brush.setColor(style.color);
    painter.setBrush(brush);    

    if(style.drawPoints)
    {
        for(int i = 0 ; i < list.size() ; i++)
            switch(style.pointStyle)
            {
            case Rhombus:
                drawRhombus(list[i], width);
                break;
            case Disc:
                drawDisc(list[i], width);
                break;
            case Square:
                drawSquare(list[i], width);
                break;
            case Triangle:
                drawTriangle(list[i], width);
                break;
            case Cross:
                drawCross(list[i], width);
                break;
            }
    }


}

void GraphDraw::drawData()
{
    for(int i = 0 ; i < information->getDataListsCount(); i++)
    {
        if(information->getDataStyle(i).draw)
            drawDataSet(i, parameters.curvesThickness+2);
    }
}

void GraphDraw::drawRegression(int reg, int width)
{
    painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);

    pen.setWidth(width);
    pen.setColor(information->getRegression(reg)->getColor());
    painter.setPen(pen);

    painter.drawPolyline(information->getRegression(reg)->getCurve());
}

void GraphDraw::drawRegressions()
{
    for(int i = 0 ; i < information->getRegressionsCount() ; i++)
    {
        if(information->getRegression(i)->getDrawState())
            drawRegression(i, parameters.curvesThickness);
    }
}

void GraphDraw::drawOneFunction(int i, int width, int curveNum)
{
    if(!funcs[i]->getDrawState())
        return;

    painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);

    short drawsNum = funcVals->at(i).size(), drawStart = 0;

    if(curveNum != -1)
    {
        drawStart = curveNum;
        drawsNum = curveNum + 1;
    }

    double posX, delta1, delta2, delta3, y1, y2, y3, y4;;
    bool pointOvershoot= false;
    int end;
    ColorSaver* colorSaver = funcs[i]->getColorSaver();

    pen.setWidth(width);

    for(short draw = drawStart ; draw < drawsNum; draw++)
    {
        end = funcVals->at(i)[draw].size();
        posX = funcValuesSaver->getStartAbscissaPixel();
        polygon.clear();

        pen.setColor(colorSaver->getColor(draw));
        painter.setPen(pen);

        for(int pos = 0; pos < end; pos++)
        {
            y1 = funcVals->at(i)[draw][pos];

            if(!isnan(y1) && !isinf(y1))
            {
                if(y1 < graphRange.Ymin || y1 > graphRange.Ymax)
                {
                    if(!pointOvershoot)
                    {
                        polygon << QPointF(posX, -y1*uniteY);
                        painter.drawPolyline(polygon);
                        polygon.clear();
                        pointOvershoot = true;
                    }
                }
                else
                {
                    if(pointOvershoot)
                    {
                       polygon << QPointF(posX - parameters.distanceBetweenPoints, - funcVals->at(i)[draw][pos-1]*uniteY);
                    }
                    polygon << QPointF(posX, -y1*uniteY);
                    pointOvershoot = false;
                }

                if(0 < pos && pos < end-2)
                {
                    y1 = funcVals->at(i)[draw][pos-1];
                    y2 = funcVals->at(i)[draw][pos];
                    y3 = funcVals->at(i)[draw][pos+1];
                    y4 = funcVals->at(i)[draw][pos+2];

                    delta1 = fabs(y2 - y1);
                    delta2 = fabs(y3 - y2);
                    delta3 = fabs(y4 - y3);

                    if(delta2 != 0 && delta2 > 2*delta1 && delta2 > 2*delta3)
                    {
                        painter.drawPolyline(polygon);
                        polygon.clear();
                    }
                }
            }
            else
            {
                painter.drawPolyline(polygon);
                polygon.clear();
            }
            posX += parameters.distanceBetweenPoints;
        }

        painter.drawPolyline(polygon);
        pointOvershoot = false;
    }
}

void GraphDraw::drawFunctions()
{
    for(int i = 0 ; i < funcs.size(); i++)
        drawOneFunction(i, parameters.curvesThickness);
}

void GraphDraw::drawOneSequence(int i, int width)
{
    if(graphRange.Xmax <= seqs[0]->get_nMin() || trunc(graphRange.Xmax) <= graphRange.Xmin || !seqs[i]->getDrawState())
        return;

     painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);
     pen.setWidth(width);

     QPointF point;
     double posX;

     if(graphRange.Xmin >  seqs[0]->get_nMin())
         posX = trunc(graphRange.Xmin);
     else posX = seqs[0]->get_nMin();

     double step = 1;

     if(uniteX < 1)
         step = 5 * trunc(1/uniteX);


     double result;
     bool ok = true;
     int end = seqs[i]->getDrawsNum();
     ColorSaver *colorSaver;

     seqs[i]->getSeqValue(trunc(graphRange.Xmax), ok);
     colorSaver = seqs[i]->getColorSaver();

     for(int k = 0; k < end; k++)
     {
         pen.setColor(colorSaver->getColor(k));
         painter.setPen(pen);

         for(double pos = posX; pos < graphRange.Xmax; pos += step)
         {
             result = seqs[i]->getSeqValue(pos, ok, k);

             if(!ok)
                 return;

             point.setX(pos * uniteX);
             point.setY(- result * uniteY);
             painter.drawPoint(point);
         }
     }
}

void GraphDraw::drawSequences()
{
    for(int i = 0 ; i < seqs.size() ; i++)
        drawOneSequence(i, parameters.curvesThickness + 3);
}

void GraphDraw::drawOneTangent(int i)
{
    if(!tangents->at(i)->isTangentValid())
        return;

    painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);

    tangents->at(i)->calculateTangentPoints(uniteX, uniteY);

    TangentPoints tangentPoints;

    pen.setColor(tangents->at(i)->getColor());

    pen.setWidth(parameters.curvesThickness);
    painter.setPen(pen);

    tangentPoints = tangents->at(i)->getCaracteristicPoints();
    painter.drawLine(QPointF(tangentPoints.left.x * uniteX, - tangentPoints.left.y * uniteY), QPointF(tangentPoints.right.x * uniteX, - tangentPoints.right.y * uniteY));

    pen.setWidth(parameters.curvesThickness + 3);
    painter.setPen(pen);

    painter.drawPoint(QPointF(tangentPoints.left.x * uniteX, - tangentPoints.left.y * uniteY));
    painter.drawPoint(QPointF(tangentPoints.center.x * uniteX,  -tangentPoints.center.y * uniteY));
    painter.drawPoint(QPointF(tangentPoints.right.x * uniteX, - tangentPoints.right.y * uniteY));
}

void GraphDraw::drawTangents()
{
    for(int i = 0 ; i < tangents->size(); i++)
    {
        if(i != tangentDrawException)
            drawOneTangent(i);
    }
}

void GraphDraw::drawStraightLines()
{
    pen.setWidth(parameters.curvesThickness);
    QPointF pt1, pt2;

    painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);

    for(int i = 0 ; i < straightLines->size(); i++)
    {
        if(!straightLines->at(i)->isValid())
            continue;

        pen.setColor(straightLines->at(i)->getColor());
        painter.setPen(pen);

        if(straightLines->at(i)->isVertical())
        {
            pt1.setX(straightLines->at(i)->getVerticalPos() * uniteX);
            pt1.setY(-graphRange.Ymax * uniteY);

            pt2.setX(straightLines->at(i)->getVerticalPos() * uniteX);
            pt2.setY(-graphRange.Ymin * uniteY);
        }
        else
        {
            pt1.setX(graphRange.Xmin * uniteX);
            pt1.setY(- straightLines->at(i)->getOrdinate(graphRange.Xmin) * uniteY);

            pt2.setX(graphRange.Xmax * uniteX);
            pt2.setY(- straightLines->at(i)->getOrdinate(graphRange.Xmax) * uniteY);
        }

        painter.drawLine(pt1, pt2);
    }
}

void GraphDraw::drawStaticParEq()
{
    QList< QList<Point> > *list;
    QPolygonF polygon;
    Point point;
    ColorSaver *colorSaver;

    pen.setWidth(parameters.curvesThickness);
    painter.setRenderHint(QPainter::Antialiasing, parameters.smoothing && !moving);
    painter.setPen(pen);

    for(int i = 0; i < parEqs->size(); i++)
    {
        if(!parEqs->at(i)->getDrawState() || parEqs->at(i)->isAnimated())
            continue;

        list = parEqs->at(i)->getPointsList();
        colorSaver = parEqs->at(i)->getColorSaver();

        for(int curve = 0; curve < list->size(); curve++)
        {
            pen.setColor(colorSaver->getColor(curve));
            painter.setPen(pen);

            polygon.clear();

            for(int pos = 0 ; pos < list->at(curve).size(); pos ++)
            {
                point = list->at(curve).at(pos);
                polygon << QPointF(point.x * uniteX, - point.y * uniteY);
            }

            painter.drawPolyline(polygon);
        }
    }
}


GraphDraw::~GraphDraw()
{
    delete funcValuesSaver;
}
