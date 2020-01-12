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

#include "GraphDraw/graphdraw.h"
#include <iostream>

using namespace std;

GraphDraw::GraphDraw(Information *info)
{
    information = info;

    coef = sqrt(3)/2;

    setMinimumSize(QSize(200, 200));
    viewSettings = info->getViewSettings();

    pen.setCapStyle(Qt::RoundCap);
    brush.setStyle(Qt::SolidPattern);

    straightLines = info->getStraightLinesList();
    tangents = info->getTangentsList();
    parEqs = info->getParEqsList();
    funcs = info->getFuncsList();
    seqs = info->getSeqsList();

    moving = false;
    tangentDrawException = -1;

    funcValuesSaver = new FuncValuesSaver(info->getFuncsList(), viewSettings.graph.distanceBetweenPoints);

    connect(information, SIGNAL(regressionAdded(Regression*)), this, SLOT(addRegSaver(Regression*)));
    connect(information, SIGNAL(regressionRemoved(Regression*)), this, SLOT(delRegSaver(Regression*)));
    connect(information, SIGNAL(newviewSettings.graph()), this, SLOT(updateSettingsVals()));
}

void GraphDraw::updateSettingsVals()
{
    viewSettings = information->getViewSettings();
    funcValuesSaver->setPixelStep(viewSettings.graph.distanceBetweenPoints);
}

void GraphDraw::addRegSaver(Regression *reg)
{
    regValuesSavers << RegressionValuesSaver(viewSettings.graph.distanceBetweenPoints, reg);
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
            case PointStyle::Rhombus:
                drawRhombus(list[i], width);
                break;
            case PointStyle::Disc:
                drawDisc(list[i], width);
                break;
            case PointStyle::Square:
                drawSquare(list[i], width);
                break;
            case PointStyle::Triangle:
                drawTriangle(list[i], width);
                break;
            case PointStyle::Cross:
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
            drawDataSet(i, viewSettings.graph.curvesThickness + 2);
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
    painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);

    for(int reg = 0 ; reg < regValuesSavers.size() ; reg++)
    {
        if(information->getRegression(reg)->getDrawState())
        {
            for(int curve = 0 ; curve < regValuesSavers[reg].getCurves().size() ; curve++)
            {
                drawCurve(viewSettings.graph.curvesThickness, information->getRegression(reg)->getColor(),
                          regValuesSavers[reg].getCurves().at(curve));
            }
        }
    }
}

void GraphDraw::recalculateRegVals()
{
    for(auto &regValSaver : regValuesSavers)
    {
        regValSaver.recalculate(Point{uniteX, uniteY}, *graphView);
    }
}


void GraphDraw::drawFunctions()
{    
    painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);

    for(int func = 0 ; func < funcs.size(); func++)
    {
        if(!funcs[func]->getDrawState())
            continue;

        for(int curve = 0 ; curve < funcValuesSaver->getFuncDrawsNum(func) ;  curve++)
            drawCurve(viewSettings.graph.curvesThickness, funcs[func]->getColorSaver()->getColor(curve), funcValuesSaver->getCurve(func, curve));
    }
}

void GraphDraw::drawOneSequence(int i, int width)
{
    if(graphView->getXmax() <= seqs[0]->get_nMin() ||
            trunc(graphView->getXmax()) <= graphView->getXmin() ||
            !seqs[i]->getDrawState())
        return;

     painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);
     pen.setWidth(width);

     QPointF point;
     double posX;

     double viewXmin = graphView->viewRect().left();
     double viewXmax = graphView->viewRect().right();

     double Xmin = graphView->getXmin();
     double Xmax = graphView->getXmax();

     if(Xmin >  seqs[0]->get_nMin())
         posX = viewXmin;
     else posX = graphView->unitToViewX(seqs[0]->get_nMin());

     double step = 1;

     if(uniteX < 1)
         step = 5 * trunc(1/uniteX);


     double result;
     bool ok = true;
     int end = seqs[i]->getDrawsNum();

     ColorSaver *colorSaver = seqs[i]->getColorSaver();


     for(int k = 0; k < end; k++)
     {
         pen.setColor(colorSaver->getColor(k));
         painter.setPen(pen);

         for(double pos = posX; pos < viewXmax; pos += step)
         {
             result = seqs[i]->getSeqValue(trunc(graphView->unitToViewX(pos)), ok, k);

             if(!ok  || !std::isnormal(result))
                 return;

             point.setX(pos);
             point.setY(result);
             painter.drawPoint(point);
         }
     }
}

void GraphDraw::drawSequences()
{
    for(int i = 0 ; i < seqs.size() ; i++)
        drawOneSequence(i, viewSettings.graph.curvesThickness + 3);
}

void GraphDraw::drawOneTangent(int i)
{
    if(!tangents->at(i)->isTangentValid())
        return;

    painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);

    tangents->at(i)->calculateTangentPoints(uniteX, uniteY);

    TangentPoints tangentPoints;

    pen.setColor(tangents->at(i)->getColor());

    pen.setWidth(viewSettings.graph.curvesThickness);
    painter.setPen(pen);

    tangentPoints = tangents->at(i)->getCaracteristicPoints();
    painter.drawLine(QPointF(graphView->unitToViewX(tangentPoints.left.x()), graphView->unitToViewY(tangentPoints.left.y())),
                     QPointF(graphView->unitToViewX(tangentPoints.right.x()), graphView->unitToViewY(tangentPoints.right.y())));

    pen.setWidth(viewSettings.graph.curvesThickness + 3);
    painter.setPen(pen);

    painter.drawPoint(QPointF(graphView->unitToViewX(tangentPoints.left.x()), graphView->unitToViewY(tangentPoints.left.y())));
    painter.drawPoint(QPointF(graphView->unitToViewX(tangentPoints.center.x()), graphView->unitToViewY(tangentPoints.center.y())));
    painter.drawPoint(QPointF(graphView->unitToViewX(tangentPoints.right.x()), graphView->unitToViewY(tangentPoints.right.y())));
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
    pen.setWidth(viewSettings.graph.curvesThickness);
    QPointF pt1, pt2;

    painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);

    for(int i = 0 ; i < straightLines->size(); i++)
    {
        if(!straightLines->at(i)->isValid())
            continue;

        pen.setColor(straightLines->at(i)->getColor());
        painter.setPen(pen);

        if(straightLines->at(i)->isVertical())
        {
            pt1.setX(graphView->unitToViewX(straightLines->at(i)->getVerticalPos()));
            pt1.setY(graphView->viewRect().top());

            pt2.setX(graphView->unitToViewX(straightLines->at(i)->getVerticalPos()));
            pt2.setY(graphView->viewRect().bottom());
        }
        else
        {
            pt1.setX(graphView->viewRect().left());
            pt1.setY(graphView->unitToViewY(straightLines->at(i)->getOrdinate(graphView->rect().left())));

            pt2.setX(graphView->viewRect().right());
            pt2.setY(graphView->unitToViewY(straightLines->at(i)->getOrdinate(graphView->rect().right())));
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

    pen.setWidth(viewSettings.graph.curvesThickness);
    painter.setRenderHint(QPainter::Antialiasing, viewSettings.graph.smoothing && !moving);
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
                polygon << QPointF(graphView->unitToViewX(point.x), graphView->unitToViewY(point.y));
            }

            painter.drawPolyline(polygon);
        }
    }
}


GraphDraw::~GraphDraw()
{
    delete funcValuesSaver;
}
