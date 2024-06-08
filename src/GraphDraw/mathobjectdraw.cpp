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

#include <iostream>

#include "GraphDraw/mathobjectdraw.h"
#include "information.h"

using namespace std;

MathObjectDraw::MathObjectDraw()
{
    coef = sqrt(3)/2;

    setMinimumSize(QSize(200, 200));

    pen.setCapStyle(Qt::RoundCap);
    brush.setStyle(Qt::SolidPattern);

    parEqs = information.getParEqsList();
    funcs = information.getFuncsList();
    seqs = information.getSeqsList();

    moving = false;

    funcValuesSaver = new FuncValuesSaver(information.getFuncsList(), information.getGraphSettings().distanceBetweenPoints);

    connect(&information, SIGNAL(regressionAdded(Regression*)), this, SLOT(addRegSaver(Regression*)));
    connect(&information, SIGNAL(regressionRemoved(Regression*)), this, SLOT(delRegSaver(Regression*)));
    connect(&information, SIGNAL(viewSettingsChanged()), this, SLOT(updateSettingsVals()));
}

void MathObjectDraw::updateSettingsVals()
{
    funcValuesSaver->setPixelStep(information.getGraphSettings().distanceBetweenPoints);
}

void MathObjectDraw::addRegSaver(Regression *reg)
{
    regValuesSavers << RegressionValuesSaver(information.getGraphSettings().distanceBetweenPoints, reg);
    recalculate = true;
    repaint();
}

void MathObjectDraw::delRegSaver(Regression *reg)
{
    for(int i = 0 ; i < regValuesSavers.size() ; i++)
        if(regValuesSavers[i].getRegression() == reg)
            regValuesSavers.removeAt(i);
    recalculate = false;
    repaint();
}

void MathObjectDraw::drawRhombus(const QPointF &pt, double w)
{
    QPolygonF polygon({pt + QPointF(-w,0), pt + QPointF(0,w), pt + QPointF(w,0), pt + QPointF(0,-w)});

    painter.drawPolygon(polygon);
}

void MathObjectDraw::drawDisc(const QPointF &pt, double w)
{
    painter.drawEllipse(pt, w, w);
}

void MathObjectDraw::drawSquare(const QPointF &pt, double w)
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    QRectF rect;
    rect.setTopLeft(pt + QPointF(-w,-w));
    rect.setBottomRight(pt + QPointF(w,w));

    painter.drawRect(rect);
}

void MathObjectDraw::drawTriangle(const QPointF &pt, double w)
{
    w*=2;
    QPolygonF polygon;
    double d  = w*coef;
    double b = w/2;

    polygon << pt + QPointF(0, -w) << pt + QPointF(d, b) << pt + QPointF(-d,b);

    painter.drawPolygon(polygon);
}

void MathObjectDraw::drawCross(const QPointF &pt, double w)
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    pen.setWidth(w);

    painter.drawLine(pt+QPointF(0,2*w), pt+QPointF(0, -2*w));
    painter.drawLine(pt+QPointF(-2*w, 0), pt+QPointF(2*w, 0));
}

void MathObjectDraw::drawDataSet(int id, int width)
{
    auto userData = information.getDataPoints(id);

    pen.setColor(userData->style.color);
    painter.setPen(pen);

    static std::vector<std::function<void(MathObjectDraw*, QPointF, double)>> draw_functions = {&MathObjectDraw::drawRhombus,
            &MathObjectDraw::drawDisc, &MathObjectDraw::drawSquare, &MathObjectDraw::drawTriangle, &MathObjectDraw::drawCross };


    const std::vector<Point> &dataPoints = userData->dataPoints;
    QPolygonF polygon;
    QPointF prev_qpt;
    bool first_pt = true;

    const double target_sq_dist = information.getGraphSettings().distanceBetweenPoints *
            information.getGraphSettings().distanceBetweenPoints;

    brush.setColor(userData->style.color);
    painter.setBrush(brush);

    for(const Point &pt: dataPoints)
    {
        QPointF qpt = userData->cartesian ?
            pt * pxPerUnit :
            Point {.x = pt.x * cos(pt.y), .y = pt.x * sin(pt.y)} * pxPerUnit;

        double &&sq_dist = QPointF::dotProduct(qpt - prev_qpt, qpt - prev_qpt);

        if(!first_pt && sq_dist <= target_sq_dist)
            continue;

        if(userData->style.drawLines)
            polygon.push_back(qpt);

        if(userData->style.drawPoints)
            draw_functions[uint(userData->style.pointStyle)](this, qpt, width);

        prev_qpt = std::move(qpt);
        first_pt = false;
    }

    if(userData->style.drawLines)
    {
        pen.setStyle(userData->style.lineStyle);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        pen.setStyle(Qt::SolidLine);
        painter.setPen(pen);
    }
}

void MathObjectDraw::drawData()
{
    for(int i = 0 ; i < information.getDataListsCount(); i++)
    {
        if(information.getDataPoints(i)->style.draw)
            drawDataSet(i, information.getGraphSettings().curvesThickness + 2);
    }
}

void MathObjectDraw::drawCurve(int width, QColor color, const QPolygonF &curve)
{
    pen.setWidth(width);
    pen.setColor(color);
    painter.setPen(pen);

    painter.drawPolyline(curve);

}

void MathObjectDraw::drawCurve(int width, QColor color, const QList<QPolygonF> &curves)
{
    for(const QPolygonF &curve: curves)
        drawCurve(width, color, curve);
}

void MathObjectDraw::drawRegressions()
{
    painter.setRenderHint(QPainter::Antialiasing, information.getGraphSettings().smoothing && !moving);

    for(int reg = 0 ; reg < regValuesSavers.size() ; reg++)
    {
        if(information.getRegression(reg)->getDrawState())
        {
            for(int curve = 0 ; curve < regValuesSavers[reg].getCurves().size() ; curve++)
            {
                drawCurve(information.getGraphSettings().curvesThickness, information.getRegression(reg)->getColor(),
                          regValuesSavers[reg].getCurves().at(curve));
            }
        }
    }
}

void MathObjectDraw::recalculateRegVals()
{
    for(auto &regValSaver : regValuesSavers)
    {
        regValSaver.recalculate(pxPerUnit, viewMapper);
    }
}


void MathObjectDraw::drawFunctions()
{
    painter.setRenderHint(QPainter::Antialiasing, information.getGraphSettings().smoothing && !moving);

    for(int func = 0 ; func < funcs.size(); func++)
    {
        if(!funcs[func]->getDrawState())
            continue;

        for(int curve = 0 ; curve < funcValuesSaver->getFuncDrawsNum(func) ;  curve++)
            drawCurve(information.getGraphSettings().curvesThickness, funcs[func]->colorSaver.getColor(curve), funcValuesSaver->getCurve(func, curve));
    }
}

void MathObjectDraw::drawOneSequence(int i, int width)
{
    {
        double real_max = viewMapper.x.getMax<zg::plane::real>().v;
        double real_min = viewMapper.x.getMin<zg::plane::real>().v;
        if (real_max <= seqs[0]->get_nMin()
            or trunc(real_max) <= real_min
            or not seqs[i]->getDrawState())
          return;
    }

    painter.setRenderHint(QPainter::Antialiasing, information.getGraphSettings().smoothing && !moving);
    pen.setWidth(width);

    zg::view_unit posX;

    zg::view_unit viewXmin = viewMapper.x.getMin<zg::plane::view>();
    zg::view_unit viewXmax = viewMapper.x.getMax<zg::plane::view>();

    zg::real_unit Xmin = viewMapper.x.getMin<zg::plane::real>();

    zg::real_unit n_min = {double(seqs[0]->get_nMin())};
    if(Xmin > n_min)
        posX = viewXmin;
    else
        posX = viewMapper.x.to<zg::plane::view>(n_min);

    zg::view_unit step = {1.};

    if(pxPerUnit.x < 1.)
        step = zg::view_unit{5 * trunc(1./pxPerUnit.x)};

    bool ok = true;
    int end = seqs[i]->getDrawsNum();

    ColorSaver *colorSaver = seqs[i]->getColorSaver();


    for(int k = 0; k < end; k++)
    {
        pen.setColor(colorSaver->getColor(k));
        painter.setPen(pen);

        for(zg::view_unit view_x = posX; view_x < viewXmax; view_x += step)
        {
            zg::real_unit real_x = viewMapper.x.to<zg::plane::real>(view_x);
            real_x.v = trunc(real_x.v);

            zg::real_unit y = {seqs[i]->getSeqValue(real_x.v, ok, k)};

            if(!ok  || !std::isfinite(y.v))
                return;

            painter.drawPoint(
              QPointF(viewMapper.to<zg::plane::pixel>(zg::point{.x = real_x, .y = y})));
        }
    }
}

void MathObjectDraw::drawSequences()
{
    for(int i = 0 ; i < seqs.size() ; i++)
        drawOneSequence(i, information.getGraphSettings().curvesThickness + 3);
}

void MathObjectDraw::drawStaticParEq()
{
    QList< QList<Point> > *list;
    QPolygonF polygon;
    Point point;
    ColorSaver *colorSaver;

    pen.setWidth(information.getGraphSettings().curvesThickness);
    painter.setRenderHint(QPainter::Antialiasing, information.getGraphSettings().smoothing && !moving);
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
                polygon << QPointF(viewMapper.to<zg::plane::pixel>(zg::real_pt::from(point)));
            }

            painter.drawPolyline(polygon);
        }
    }
}


MathObjectDraw::~MathObjectDraw()
{
    delete funcValuesSaver;
}
