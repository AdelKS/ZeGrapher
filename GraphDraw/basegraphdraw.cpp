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

#include "GraphDraw/basegraphdraw.h"


BaseGraphDraw::BaseGraphDraw(Information *info) : MathObjectDraw(info), gridCalculator(info, this)
{
    information = info;
    leftMargin = 30;
    rightMargin = 30;
    topMargin = 20;
    bottomMargin = 30;

    tangentDrawException = -1;
    legendFontSize = 12;
    legendState = false;
    additionalMargin = 0;
    bold = italic = underline = false;
    numPrec = NUM_PREC;
    viewMapper.setGraphRange(information->getGraphRange());

    connect(information, SIGNAL(graphRangeChanged(GraphRange)), &viewMapper, SLOT(setGraphRange(GraphRange)));
    connect(information, SIGNAL(graphRangeChanged(GraphRange)), this, SLOT(update()));
    connect(information, SIGNAL(updateOccured()), this, SLOT(update()));
}

void BaseGraphDraw::setNumPrec(int prec)
{
    numPrec = prec;
    update();
}

void BaseGraphDraw::setBold(bool state)
{
    bold = state;
    update();
}

void BaseGraphDraw::setUnderline(bool state)
{
    underline = state;
    update();
}

void BaseGraphDraw::setItalic(bool state)
{
    italic = state;
    update();
}

void BaseGraphDraw::setlegendFontSize(int size)
{
    if(legendState)
    {
        leftMargin -= legendFontSize;
        leftMargin += size;

        bottomMargin -= legendFontSize;
        bottomMargin += size;

        additionalMargin = size + 10;
    }

    legendFontSize = size;

    update();
}

void BaseGraphDraw::setLegendState(bool show)
{
    if(show && !legendState)
    {
        leftMargin += legendFontSize + 10;
        bottomMargin += legendFontSize + 10;

        additionalMargin = legendFontSize + 10;

    }
    else if(!show && legendState)
    {
       leftMargin -= legendFontSize + 10;
       bottomMargin -= legendFontSize + 10;

       additionalMargin = 0;
    }

    legendState = show;

    update();
}

void BaseGraphDraw::setXaxisLegend(QString legend)
{
    xLegend = legend;
    update();
}

void BaseGraphDraw::setYaxisLegend(QString legend)
{
    yLegend = legend;
    update();
}

void BaseGraphDraw::updateGraphRect()
{    
    graphRectScaled.setWidth(figureRectScaled.width() - leftMargin - rightMargin);
    graphRectScaled.setHeight(figureRectScaled.height() - topMargin - bottomMargin);
    graphRectScaled.moveTopLeft(QPoint(0, 0)); // because painter is translated to its top-left corner
}

void BaseGraphDraw::paint()
{
    updateGraphRect(); // must happen before the next instruction
    updateCenterPosAndScaling();

    painter.translate(leftMargin, topMargin);

    drawBaseGraph();

    if(legendState)
        writeLegends();

    painter.setClipRect(graphRectScaled);

    painter.translate(QPointF(centre.x, centre.y));

    funcValuesSaver->calculateAll(uniteX, uniteY, viewMapper);
    recalculateRegVals();

    drawFunctions();
    drawSequences();
    drawStraightLines();
    drawTangents();
    drawStaticParEq();
    drawRegressions();
    drawData();
}

void BaseGraphDraw::writeLegends()
{
    font = information->getGraphSettings().estheticSettings.graphFont;
    font.setPixelSize(legendFontSize);
    font.setItalic(italic);
    font.setBold(bold);
    font.setUnderline(underline);

    painter.setFont(font);

    if(!xLegend.isEmpty())
    {
        int xLegendWidth = painter.fontMetrics().width(xLegend);

        QPoint startDrawPoint;
        startDrawPoint.setX((graphRectScaled.width() - xLegendWidth)/2);
        startDrawPoint.setY(graphRectScaled.height() + bottomMargin - 10);

        painter.drawText(startDrawPoint, xLegend);
    }
    if(!yLegend.isEmpty())
    {
        painter.rotate(-90);
        int yLegendWidth = painter.fontMetrics().width(yLegend);
        int yLegendHeight = legendFontSize + 6;

        QPoint startDrawPoint;
        startDrawPoint.setX(- (graphRectScaled.height() - (graphRectScaled.height() - yLegendWidth)/2));
        startDrawPoint.setY(-leftMargin + yLegendHeight);

        painter.drawText(startDrawPoint, yLegend);

        painter.rotate(90);
    }
}

void BaseGraphDraw::drawLinAxisGridTicksX()
{
    painter.setFont(information->getGraphSettings().estheticSettings.graphFont);
    QFontMetrics fontMetrics = painter.fontMetrics();

    double space, pos;
    double Xpos;

    QString num;

    ZeLinAxisTicks xAxisTicks = gridCalculator.getLinearAxisTicks(graphRectScaled.width(),
                                                             viewMapper.getGraphRange().x,
                                                             ZeAxisName::X,
                                                             fontMetrics);

    const ZeAxisSettings &axisSettings = information->getAxesSettings().x;
    const Ze1DGridSettings &gridSettings = information->getGridSettings().x;

    pen.setCapStyle(Qt::FlatCap);

    for(ZeLinAxisTick &axisTick: xAxisTicks.ticks)
    {
        Xpos = axisTick.pos * uniteX;

        if(fabs(Xpos) > 1)
        {
            if(gridSettings.showGrid)
            {
                pen.setColor(gridSettings.gridColor);
                pen.setWidthF(gridSettings.gridLineWidth);
                painter.setPen(pen);
                painter.setRenderHint(QPainter::Antialiasing, false);
                painter.drawLine(QPointF(Xpos + centre.x, 0), QPointF(Xpos + centre.x, graphRectScaled.height()));
                painter.setRenderHint(QPainter::Antialiasing, true);
            }
            pen.setColor(axisSettings.color);
            pen.setWidthF(axisSettings.lineWidth);
            painter.setPen(pen);

            pos = Xpos + centre.x;

            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawLine(QPointF(pos, 4), QPointF(pos, 0));
            painter.drawLine(QPointF(pos, graphRectScaled.height()-4), QPointF(pos, graphRectScaled.height()));
            painter.setRenderHint(QPainter::Antialiasing, true);

            num = QString::number(axisTick.pos, 'g', numPrec);
            space = fontMetrics.width(num);
            painter.drawText(QPointF(pos - space/2, graphRectScaled.height()+15), num);
        }
        else
        {
            pos = Xpos + centre.x;
            space = fontMetrics.width("0");
            painter.drawText(QPointF(pos - space/2, graphRectScaled.height()+15), "0");
        }
    }
}

void BaseGraphDraw::drawLinAxisGridTicksY()
{
    painter.setFont(information->getGraphSettings().estheticSettings.graphFont);
    QFontMetrics fontMetrics = painter.fontMetrics();

    double pos;
    int space, largestWidth = 0;
    double Ypos;

    QString num;

    ZeLinAxisTicks yAxisTicks = gridCalculator.getLinearAxisTicks(graphRectScaled.height(),
                                                             viewMapper.getGraphRange().y,
                                                             ZeAxisName::Y,
                                                             fontMetrics);

    const auto &axisSettings = information->getAxesSettings().y;
    const auto &gridSettings = information->getGridSettings().y;

    pen.setCapStyle(Qt::FlatCap);

    for(ZeLinAxisTick &axisTick: yAxisTicks.ticks)
    {
        Ypos = axisTick.pos * uniteY;

        if(fabs(Ypos) > 1)
        {
            if(gridSettings.showGrid)
            {
                pen.setColor(gridSettings.gridColor);
                pen.setWidthF(gridSettings.gridLineWidth);
                painter.setPen(pen);
                painter.setRenderHint(QPainter::Antialiasing, false);
                painter.drawLine(QPointF(0, -Ypos + centre.y), QPointF(graphRectScaled.width(), -Ypos + centre.y));
                painter.setRenderHint(QPainter::Antialiasing, true);
            }

            pen.setColor(axisSettings.color);
            pen.setWidth(axisSettings.lineWidth);
            painter.setPen(pen);

            pos = -Ypos + centre.y;

            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawLine(QPointF(4, pos), QPointF(0, pos));
            painter.drawLine(QPointF(graphRectScaled.width() - 4, pos), QPointF(graphRectScaled.width(), pos));
            painter.setRenderHint(QPainter::Antialiasing, true);

            num = QString::number(Ypos/uniteY, 'g', numPrec);
            space = fontMetrics.width(num) + 5;

            if(space > largestWidth)
                largestWidth = space;

            painter.drawText(QPointF(-space, pos + fontMetrics.height() /2), num);
        }
        else
        {
            pos = -Ypos + centre.y;
            space = fontMetrics.width("0") + 5;
            painter.drawText(QPointF(-space, pos + fontMetrics.height()/2), "0");
        }

        if(space > largestWidth)
            largestWidth = space;
    }

    if(leftMargin - additionalMargin - largestWidth > 8 || leftMargin - additionalMargin - largestWidth < 4)
    {
        leftMargin = largestWidth + additionalMargin + 6;
        update();
    }
}

void BaseGraphDraw::drawBaseGraph()
{
    drawAxes();
    if(information->getAxesSettings().x.axisType == ZeAxisType::LINEAR)
        drawLinAxisGridTicksX();

    if(information->getAxesSettings().y.axisType == ZeAxisType::LINEAR)
        drawLinAxisGridTicksY();
}

void BaseGraphDraw::drawAxes()
{
    const auto &graphRange = information->getGraphRange();
    const auto &axesSettings = information->getAxesSettings();

    const auto &viewRect = viewMapper.getViewRect();

    if(viewRect.left() < 0 && viewRect.right() > 0)
    {
        pen.setWidth(axesSettings.x.lineWidth);
        pen.setColor(axesSettings.x.color);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, false);

        painter.drawLine(QPointF(0, centre.y), QPointF(graphRectScaled.width(), centre.y));
    }
    if(viewRect.bottom() < 0 && viewRect.top() > 0)
    {
        pen.setWidth(axesSettings.y.lineWidth);
        pen.setColor(axesSettings.y.color);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, false);

        painter.drawLine(QPointF(centre.x, 0), QPointF(centre.x, graphRectScaled.height()));
    }

    painter.drawRect(graphRectScaled);
}

void BaseGraphDraw::updateCenterPosAndScaling()
{
    // TODO: update this method not working here

    uniteY = double(graphRectScaled.height()) / fabs(viewMapper.getViewRect().height());
    uniteX = double(graphRectScaled.width()) / fabs(viewMapper.getViewRect().width());

    double rapport = uniteY / uniteX;

    if(information->getAxesSettings().orthonormal)
    {
        // TODO
    }

    centre.x = - viewMapper.getViewRect().left() * uniteX;
    centre.y =  viewMapper.getViewRect().top() * uniteY;
}

QImage* BaseGraphDraw::drawImage()
{
    //TODO: update this method

    viewSettings = information->getViewSettings();

    QImage *image = new QImage(size(), QImage::Format_RGB32);
    image->fill(viewSettings.graph.estheticSettings.backgroundColor.rgb());

    painter.begin(image);
    //trace du background

    pen.setColor(viewSettings.axes.x.color);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawRect(leftMargin, topMargin, graphRectScaled.width(), graphRectScaled.height());

    painter.translate(leftMargin, topMargin);

    updateCenterPosAndScaling();
    drawBaseGraph();

    if(legendState)
        writeLegends();

    painter.setClipRect(0,0,graphRectScaled.width(),graphRectScaled.height());

    painter.translate(QPointF(centre.x, centre.y));

    if(recalculate)
    {
        funcValuesSaver->calculateAll(uniteX, uniteY, viewMapper);
        recalculateRegVals();
    }

    drawFunctions();
    drawSequences();
    drawStraightLines();
    drawStaticParEq();
    drawRegressions();
    drawData();

    painter.end();

    //*image = image->convertToFormat(QImage::Format_Indexed8, Qt::DiffuseDither);
    return image;
}
