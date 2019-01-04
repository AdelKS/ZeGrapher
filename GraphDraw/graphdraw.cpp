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

#include "GraphDraw/graphdraw.h"
#include <iostream>

using namespace std;

GraphDraw::GraphDraw(Information *info)
{
    information = info;

    coef = sqrt(3)/2;

    graphSettings = info->getSettingsVals();
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

    funcValuesSaver = new FuncValuesSaver(info->getFuncsList(), information->getSettingsVals().distanceBetweenPoints);

    connect(information, SIGNAL(regressionAdded(Regression*)), this, SLOT(addRegSaver(Regression*)));
    connect(information, SIGNAL(regressionRemoved(Regression*)), this, SLOT(delRegSaver(Regression*)));
    connect(information, SIGNAL(newSettingsVals()), this, SLOT(updateSettingsVals()));
}

void GraphDraw::updateSettingsVals()
{
    funcValuesSaver->setPixelStep(information->getSettingsVals().distanceBetweenPoints);
}

void GraphDraw::addRegSaver(Regression *reg)
{
    regValuesSavers << RegressionValuesSaver(information->getSettingsVals().distanceBetweenPoints, reg);
    recalculate = true;
    repaint();
}

void GraphDraw::delRegSaver(Regression *reg)
{
    for(int i = 0 ; i < regValuesSavers.size() ; i++)
        if(regValuesSavers[i].getRegression() == reg)
            regValuesSavers.removeAt(i);
    recalculate = false;
    repaint();
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
            drawDataSet(i, graphSettings.curvesThickness+2);
    }
}

void GraphDraw::drawCurve(int width, QColor color, const QPolygonF &curve)
{
    pen.setWidth(width);
    pen.setColor(color);
    painter.setPen(pen);

    painter.drawPolyline(curve);

}

void GraphDraw::drawCurve(int width, QColor color, const QList<QPolygonF> &curves)
{
    for(QPolygonF curve: curves)
        drawCurve(width, color, curve);
}

void GraphDraw::drawRegressions()
{
    painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);

    for(int reg = 0 ; reg < regValuesSavers.size() ; reg++)
    {
        if(information->getRegression(reg)->getDrawState())
        {
            for(int curve = 0 ; curve < regValuesSavers[reg].getCurves().size() ; curve++)
            {
                drawCurve(graphSettings.curvesThickness, information->getRegression(reg)->getColor(),
                          regValuesSavers[reg].getCurves().at(curve));
            }
        }
    }
}

void GraphDraw::recalculateRegVals()
{
    for(auto &regValSaver : regValuesSavers)
    {
        regValSaver.recalculate(Point{uniteX, uniteY}, graphRange);
    }
}


void GraphDraw::drawFunctions()
{    
    painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);

    for(int func = 0 ; func < funcs.size(); func++)
    {
        if(!funcs[func]->getDrawState())
            continue;

        for(int curve = 0 ; curve < funcValuesSaver->getFuncDrawsNum(func) ;  curve++)
            drawCurve(graphSettings.curvesThickness, funcs[func]->getColorSaver()->getColor(curve), funcValuesSaver->getCurve(func, curve));
    }
}

void GraphDraw::drawOneSequence(int i, int width)
{
    if(graphRange.Xmax <= seqs[0]->get_nMin() || trunc(graphRange.Xmax) <= graphRange.Xmin || !seqs[i]->getDrawState())
        return;

     painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);
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

             if(!ok  || !std::isnormal(result))
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
        drawOneSequence(i, graphSettings.curvesThickness + 3);
}

void GraphDraw::drawOneTangent(int i)
{
    if(!tangents->at(i)->isTangentValid())
        return;

    painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);

    tangents->at(i)->calculateTangentPoints(uniteX, uniteY);

    TangentPoints tangentPoints;

    pen.setColor(tangents->at(i)->getColor());

    pen.setWidth(graphSettings.curvesThickness);
    painter.setPen(pen);

    tangentPoints = tangents->at(i)->getCaracteristicPoints();
    painter.drawLine(QPointF(tangentPoints.left.x * uniteX, - tangentPoints.left.y * uniteY), QPointF(tangentPoints.right.x * uniteX, - tangentPoints.right.y * uniteY));

    pen.setWidth(graphSettings.curvesThickness + 3);
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
    pen.setWidth(graphSettings.curvesThickness);
    QPointF pt1, pt2;

    painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);

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

    pen.setWidth(graphSettings.curvesThickness);
    painter.setRenderHint(QPainter::Antialiasing, graphSettings.smoothing && !moving);
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
