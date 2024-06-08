/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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
#include "information.h"

BaseGraphDraw::BaseGraphDraw() : MathObjectDraw(), gridCalculator(this)
{

    leftMargin = 30;
    rightMargin = 30;
    topMargin = 20;
    bottomMargin = 30;

    legendFontSize = 12;
    legendState = false;
    additionalMargin = 0;
    bold = italic = underline = false;
    numPrec = NUM_PREC;
    viewMapper.setGraphRange(information.getGraphRange());

    connect(&information, SIGNAL(graphRangeChanged(GraphRange)), this, SLOT(graphRangeChanged(const GraphRange&)));
    connect(&information, SIGNAL(estheticSettingsChanged()), this, SLOT(update()));
    connect(&information, SIGNAL(updateOccured()), this, SLOT(update()));
}

void BaseGraphDraw::graphRangeChanged(const GraphRange& range)
{
    viewMapper.setGraphRange(range);
    update();
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
    viewMapper.setGraphRect(graphRectScaled);
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

    funcValuesSaver->calculateAll(pxPerUnit, viewMapper);
    recalculateRegVals();

    drawFunctions();
    drawSequences();
    drawStaticParEq();
    drawRegressions();
    drawData();
}

void BaseGraphDraw::writeLegends()
{
    font = information.getGraphSettings().graphFont;
    font.setPixelSize(legendFontSize);
    font.setItalic(italic);
    font.setBold(bold);
    font.setUnderline(underline);

    painter.setFont(font);

    if(!xLegend.isEmpty())
    {
        int xLegendWidth = painter.fontMetrics().boundingRect(xLegend).width();

        QPoint startDrawPoint;
        startDrawPoint.setX((graphRectScaled.width() - xLegendWidth)/2);
        startDrawPoint.setY(graphRectScaled.height() + bottomMargin - 10);

        painter.drawText(startDrawPoint, xLegend);
    }
    if(!yLegend.isEmpty())
    {
        painter.rotate(-90);
        int yLegendWidth = painter.fontMetrics().boundingRect(yLegend).width();
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
    painter.setFont(information.getGraphSettings().graphFont);
    QFontMetrics fontMetrics = painter.fontMetrics();

    double space, pos;
    double Xpos;

    ZeLinAxisTicks xAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.x, fontMetrics);

    const ZeAxesSettings &axesSettings = information.getAxesSettings();
    const Ze1DGridSettings &gridSettings = information.getGridSettings().x;

    pen.setCapStyle(Qt::FlatCap);
    bool first_tick = true;
    double previous_pos = 0;

    painter.setFont(information.getGraphSettings().graphFont);
    double text_height = fontMetrics.boundingRect('0').height();

    for(const ZeLinAxisTick &axisTick: xAxisTicks.ticks)
    {
        if(not (information.getGraphRange().x.min < axisTick.pos && axisTick.pos < information.getGraphRange().x.max))
            continue;

        Xpos = axisTick.pos * pxPerUnit.x;
        pos = Xpos + centre.x;

        if (fabs(Xpos) > 1)
        {
            if(gridSettings.showGrid)
            {
                pen.setColor(gridSettings.gridColor);
                pen.setWidthF(gridSettings.gridLineWidth);
                painter.setPen(pen);
                painter.setRenderHint(QPainter::Antialiasing, false);
                painter.drawLine(QPointF(pos, 0), QPointF(pos, graphRectScaled.height()));
            }
            pen.setColor(axesSettings.color);
            pen.setWidthF(axesSettings.lineWidth);
            painter.setPen(pen);

            pos = Xpos + centre.x;

            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawLine(QPointF(pos, 4), QPointF(pos, 0));
            painter.drawLine(QPointF(pos, graphRectScaled.height()-4), QPointF(pos, graphRectScaled.height()));

            if(axisTick.posStr.startsWith('-'))
                space = fontMetrics.boundingRect(axisTick.posStr.mid(1)).width()/2 + fontMetrics.horizontalAdvance('-');
            else space = fontMetrics.boundingRect(axisTick.posStr).width()/2;
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawText(QPointF(pos - space, graphRectScaled.height() + text_height + 5), axisTick.posStr);
        }
        else
        {
            pen.setColor(axesSettings.color);
            pen.setWidth(axesSettings.lineWidth);
            painter.setPen(pen);

            space = fontMetrics.boundingRect("0").width()/2;
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawText(QPointF(pos - space, graphRectScaled.height() + text_height + 5), "0");
        }


        if(gridSettings.showSubGrid && !first_tick)
        {
            pen.setColor(gridSettings.subgridColor);
            pen.setWidthF(gridSettings.subgridLineWidth);
            painter.setPen(pen);
            painter.setRenderHint(QPainter::Antialiasing, false);

            for(uint mul = 1 ; mul <= gridSettings.subgridSubDivs ; mul++)
            {
                const double cur_pos = double(mul) * previous_pos / double(gridSettings.subgridSubDivs + 1) +
                        double(gridSettings.subgridSubDivs + 1 - mul) * axisTick.pos / double(gridSettings.subgridSubDivs + 1);

                if(information.getGraphRange().x.min < cur_pos && cur_pos < information.getGraphRange().x.max)
                    painter.drawLine(QPointF(cur_pos * pxPerUnit.x + centre.x, 0), QPointF(cur_pos * pxPerUnit.x + centre.x, graphRectScaled.height()));

            }
        }

        previous_pos = axisTick.pos;
        first_tick = false;
    }

    int old_rightMargin = rightMargin;
    if(xAxisTicks.offset.sumOffset != 0)
    {
        QString sum_offset;
        if(xAxisTicks.offset.sumPowerOffset == 0)
            sum_offset = " + " + QString::number(xAxisTicks.offset.sumOffset, 'g', 11);
        else sum_offset = " + " + QString::number(xAxisTicks.offset.sumOffset * int_pow(10.0, -xAxisTicks.offset.sumPowerOffset), 'g', 11) +
                "×10^" + QString::number(xAxisTicks.offset.sumPowerOffset);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawText(QPointF(graphRectScaled.width() + 5, graphRectScaled.height() - text_height - 10), sum_offset);

        int margin = fontMetrics.boundingRect(sum_offset).width() + 5;
        if(rightMargin < margin)
            rightMargin = 10 + margin;
    }
    if(xAxisTicks.offset.basePowerOffset != 0)
    {
        QString power_offset = "×10^" + QString::number(xAxisTicks.offset.basePowerOffset);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawText(QPointF(graphRectScaled.width() + 5, graphRectScaled.height()), power_offset);

        int margin = fontMetrics.boundingRect(power_offset).width() + 5;
        if(rightMargin < margin)
            rightMargin = 10 + margin;
    }

    if(xAxisTicks.offset.basePowerOffset == 0 && xAxisTicks.offset.sumOffset == 0)
        rightMargin = 10;

    if(rightMargin != old_rightMargin)
        update();
}


void BaseGraphDraw::drawLinAxisGridTicksY()
{
    painter.setFont(information.getGraphSettings().graphFont);
    QFontMetrics fontMetrics = painter.fontMetrics();

    double pos;
    int space, largestWidth = 0;
    double Ypos;

    ZeLinAxisTicks yAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.y, fontMetrics);

    const auto &axesSettings = information.getAxesSettings();
    const auto &gridSettings = information.getGridSettings().y;

    pen.setCapStyle(Qt::FlatCap);
    bool first_tick = true;
    double previous_pos = 0;

    painter.setFont(information.getGraphSettings().graphFont);
    double text_height = fontMetrics.boundingRect('0').height();

    for(const ZeLinAxisTick &axisTick: yAxisTicks.ticks)
    {
        Ypos = axisTick.pos * pxPerUnit.y;

        if(information.getGraphRange().y.min < axisTick.pos && axisTick.pos < information.getGraphRange().y.max)
        {
            pos = graphRectScaled.height() - (Ypos + centre.y);
            if(fabs(Ypos) > 1)
            {
                if(gridSettings.showGrid)
                {
                    pen.setColor(gridSettings.gridColor);
                    pen.setWidthF(gridSettings.gridLineWidth);
                    painter.setPen(pen);
                    painter.setRenderHint(QPainter::Antialiasing, false);
                    painter.drawLine(QPointF(0, pos), QPointF(graphRectScaled.width(), pos));
                }

                pen.setColor(axesSettings.color);
                pen.setWidth(axesSettings.lineWidth);
                painter.setPen(pen);

                painter.setRenderHint(QPainter::Antialiasing, false);
                painter.drawLine(QPointF(4, pos), QPointF(0, pos));
                painter.drawLine(QPointF(graphRectScaled.width() - 4, pos), QPointF(graphRectScaled.width(), pos));

                space = fontMetrics.boundingRect(axisTick.posStr).width() + 5;

                if(space > largestWidth)
                    largestWidth = space;

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.drawText(QPointF(-space, pos + text_height/2), axisTick.posStr);
            }
            else
            {
                pen.setColor(axesSettings.color);
                pen.setWidth(axesSettings.lineWidth);
                painter.setPen(pen);

                space = fontMetrics.boundingRect("0").width() + 5;
                painter.drawText(QPointF(-space, pos + text_height/2), "0");
            }

            if(space > largestWidth)
                largestWidth = space;
        }

        if(gridSettings.showSubGrid && !first_tick)
        {
            pen.setColor(gridSettings.subgridColor);
            pen.setWidthF(gridSettings.subgridLineWidth);
            painter.setPen(pen);
            painter.setRenderHint(QPainter::Antialiasing, false);

            for(uint mul = 1 ; mul <= gridSettings.subgridSubDivs ; mul++)
            {
                const double cur_pos = double(mul) * previous_pos / double(gridSettings.subgridSubDivs + 1) +
                        double(gridSettings.subgridSubDivs + 1 - mul) * axisTick.pos / double(gridSettings.subgridSubDivs + 1);

                if(information.getGraphRange().y.min < cur_pos && cur_pos < information.getGraphRange().y.max)
                    painter.drawLine(QPointF(0, cur_pos * pxPerUnit.y + centre.y), QPointF(graphRectScaled.width(), cur_pos * pxPerUnit.y + centre.y));
            }
        }

        previous_pos = axisTick.pos;
        first_tick = false;
    }

    int old_topMargin = topMargin;
    int powerOffset_size = 0;
    int offset_margin = 0;
    if(yAxisTicks.offset.basePowerOffset != 0)
    {
        QString power_offset = "×10^" + QString::number(yAxisTicks.offset.basePowerOffset);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawText(QPointF(0, -4), power_offset);

        offset_margin = fontMetrics.boundingRect(power_offset).height();

        powerOffset_size = fontMetrics.boundingRect(power_offset).width() + 5;
    }
    if(yAxisTicks.offset.sumOffset != 0)
    {
        QString sum_offset;
        if(yAxisTicks.offset.sumPowerOffset == 0)
            sum_offset = " + " + QString::number(yAxisTicks.offset.sumOffset, 'g', 11);
        else sum_offset = " + " + QString::number(yAxisTicks.offset.sumOffset * int_pow(10.0, -yAxisTicks.offset.sumPowerOffset), 'g', 11) +
                "×10^" + QString::number(yAxisTicks.offset.sumPowerOffset);

        int new_offsetmargin = fontMetrics.boundingRect(sum_offset).height();
        if(new_offsetmargin > offset_margin)
            offset_margin = new_offsetmargin;

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawText(QPointF(powerOffset_size + 5, -4), sum_offset);
    }

    if(yAxisTicks.offset.basePowerOffset == 0 && yAxisTicks.offset.sumOffset == 0)
        topMargin = 20;
    else topMargin = 5 + offset_margin;

    if(leftMargin - additionalMargin - largestWidth > 8 || leftMargin - additionalMargin - largestWidth < 4)
    {
        leftMargin = largestWidth + additionalMargin + 6;
        update();
    }
    else if(topMargin != old_topMargin)
        update();
}

void BaseGraphDraw::drawBaseGraph()
{
    if(information.getAxesSettings().x.axisType == ZeViewType::LINEAR)
        drawLinAxisGridTicksX();

    if(information.getAxesSettings().y.axisType == ZeViewType::LINEAR)
        drawLinAxisGridTicksY();

    drawAxes();
}

void BaseGraphDraw::drawAxes()
{
    const auto &axesSettings = information.getAxesSettings();

    const auto &viewRect = viewMapper.getRange<zg::plane::view>();

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setBrush(QBrush(Qt::NoBrush));

    pen.setWidth(axesSettings.lineWidth);
    pen.setColor(axesSettings.color);
    painter.setPen(pen);

    painter.drawRect(graphRectScaled);

    if(viewRect.x.min.v < 0 && viewRect.x.max.v > 0)
    {
        pen.setWidth(axesSettings.lineWidth);
        pen.setColor(axesSettings.color);
        painter.setPen(pen);

        painter.drawLine(QPointF(0, graphRectScaled.height() - centre.y),
                         QPointF(graphRectScaled.width(), graphRectScaled.height() - centre.y));
    }
    if(viewRect.y.min.v < 0 && viewRect.y.max.v > 0)
    {
        pen.setWidth(axesSettings.lineWidth);
        pen.setColor(axesSettings.color);
        painter.setPen(pen);

        painter.drawLine(QPointF(centre.x, 0), QPointF(centre.x, graphRectScaled.height()));
    }
}

void BaseGraphDraw::updateCenterPosAndScaling()
{
    // TODO: update this method not working here

    pxPerUnit.y = double(graphRectScaled.height()) / fabs(std::as_const(viewMapper).y.getRange<zg::plane::view>().amplitude().v);
    pxPerUnit.x = double(graphRectScaled.width()) / fabs(std::as_const(viewMapper).x.getRange<zg::plane::view>().amplitude().v);

    if(information.getAxesSettings().orthonormal)
    {
        // TODO
    }

    centre.x = - viewMapper.x.getMin<zg::plane::view>().v * pxPerUnit.x;
    centre.y = -  viewMapper.y.getMin<zg::plane::view>().v * pxPerUnit.y;
}

QImage* BaseGraphDraw::drawImage()
{
    //TODO: update this method

    QImage *image = new QImage(size(), QImage::Format_RGB32);
    image->fill(information.getGraphSettings().backgroundColor.rgb());

    painter.begin(image);
    //trace du background

    pen.setColor(information.getAxesSettings().color);
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
        funcValuesSaver->calculateAll(pxPerUnit, viewMapper);
        recalculateRegVals();
    }

    drawFunctions();
    drawSequences();
    drawStaticParEq();
    drawRegressions();
    drawData();

    painter.end();

    //*image = image->convertToFormat(QImage::Format_Indexed8, Qt::DiffuseDither);
    return image;
}
