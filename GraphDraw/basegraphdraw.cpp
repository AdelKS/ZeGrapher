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

#include "GraphDraw/imagepreview.h"


ImagePreview::ImagePreview(Information *info) : GraphDraw(info)
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

    connect(information, SIGNAL(updateOccured()), this, SLOT(update()));
    connect(information, SIGNAL(gridStateChange()), this, SLOT(update()));

}

void ImagePreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    viewSettings = information->getViewSettings();

    painter.begin(this);

    painter.setFont(information->getGraphSettings().graphFont);

    painter.setBrush(QBrush(viewSettings.graph.backgroundColor));
    painter.drawRect(-1, -1, width()+1, height()+1);

    updateGraphRect();
    updateCenterPosAndScaling();
    funcValuesSaver->calculateAll(uniteX, uniteY, *graphView);

    paint();

    painter.end();

}

void ImagePreview::setNumPrec(int prec)
{
    numPrec = prec;
    update();
}

void ImagePreview::setBold(bool state)
{
    bold = state;
    update();
}

void ImagePreview::setUnderline(bool state)
{
    underline = state;
    update();
}

void ImagePreview::setItalic(bool state)
{
    italic = state;
    update();
}

void ImagePreview::setlegendFontSize(int size)
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

void ImagePreview::setLegendState(bool show)
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

void ImagePreview::setXaxisLegend(QString legend)
{
    xLegend = legend;
    update();
}

void ImagePreview::setYaxisLegend(QString legend)
{
    yLegend = legend;
    update();
}

void ImagePreview::updateGraphRect()
{    
    graphRectScaled.setWidth(figureRectScaled.width() - leftMargin - rightMargin);
    graphRectScaled.setHeight(figureRectScaled.height() - topMargin - bottomMargin);
    graphRectScaled.moveTopLeft(QPoint(0, 0)); // because painter is translated to its top-left corner
}

void ImagePreview::paint()
{
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);    

    pen.setColor(viewSettings.axes.x.color);
    painter.setPen(pen);

    updateGraphRect(); // must happen before the next instruction
    updateCenterPosAndScaling();

    painter.translate(leftMargin, topMargin);

    drawTicksAndNumbers();
    drawAxes();    

    if(legendState)
        writeLegends();

    painter.setClipRect(graphRectScaled);

    painter.translate(QPointF(centre.x, centre.y));

    funcValuesSaver->calculateAll(uniteX, uniteY, *graphView);
    recalculateRegVals();

    drawFunctions();
    drawSequences();
    drawStraightLines();
    drawTangents();
    drawStaticParEq();
    drawRegressions();
    drawData();
}

void ImagePreview::writeLegends()
{
    font = information->getGraphSettings().graphFont;
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

void ImagePreview::drawTicksAndNumbers()
{
    // TODO: update this method

    double fontSize = information->getGraphSettings().graphFont.pixelSize();
    double prec = numPrec;

    font.setPixelSize(fontSize);
    font.setItalic(false);
    font.setBold(false);
    font.setUnderline(false);    

    pen.setWidth(1);
    painter.setFont(viewSettings.graph.graphFont);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QFontMetrics fontMetrics = painter.fontMetrics();

    double space, pos;



    double Xpos;

    ZeGrid grid = graphView.getGrid();

    QList<ZeMainGridLine> &horMainGrid = grid.horizontal.mainGrid;


    QString num;

    for(ZeMainGridLine gridLine: horMainGrid)
    {
        Xpos = gridLine.pos;

        if(fabs(Xpos) > 1)
        {
            if(graphSettings.gridSettings.gridType == ZeGridType::GRID)
            {
                pen.setColor(graphSettings.gridColor);
                pen.setWidthF(0.5);
                painter.setPen(pen);
                painter.drawLine(QPointF(Xpos + centre.x, 0), QPointF(Xpos + centre.x, graphRectScaled.height()));
            }
            pen.setColor(graphSettings.axesColor);

            painter.setPen(pen);

            pos = Xpos + centre.x;

            painter.drawLine(QPointF(pos, 4), QPointF(pos, 0));
            painter.drawLine(QPointF(pos, graphRectScaled.height()-4), QPointF(pos, graphRectScaled.height()));
            num = QString::number(Xpos/uniteX, 'g', numPrec);
            space = fontMetrics.width(num);
            painter.drawText(QPointF(pos - space/2, graphRectScaled.height()+15), num);
        }
        else
        {
            pos = Xpos + centre.x;
            space = fontMetrics.width("0");
            painter.drawText(QPointF(pos - space/2, graphRectScaled.height()+15), "0");
        }

        Xpos += step;
    }

//trace sur l'axe des Y

    double Ypos = ceil(graphView.Ymin / graphSettings.gridSettings.yGridStep) * graphSettings.gridSettings.yGridStep * uniteY;
    step = graphSettings.gridSettings.yGridStep * uniteY;
    end = graphView.Ymax * uniteY;

    int largestWidth = 0;

    while(Ypos <= end)
    {
        if(fabs(Ypos) > 1)
        {
            if(graphSettings.gridSettings.gridType == ZeGridType::GRID)
            {
                pen.setColor(graphSettings.gridColor);
                pen.setWidthF(0.5);
                painter.setPen(pen);
                painter.drawLine(QPointF(0, -Ypos + centre.y), QPointF(graphRectScaled.width(), -Ypos + centre.y));
            }

            pen.setColor(graphSettings.axesColor);
            pen.setWidth(1);
            painter.setPen(pen);

            pos = -Ypos + centre.y;

            painter.drawLine(QPointF(4, pos), QPointF(0, pos));
            painter.drawLine(QPointF(graphRectScaled.width() - 4, pos), QPointF(graphRectScaled.width(), pos));

            num = QString::number(Ypos/uniteY, 'g', numPrec);
            space = fontMetrics.width(num) + 5;

            if(space > largestWidth)
                largestWidth = space;

            painter.drawText(QPointF(-space, pos + graphSettings.graphFont.pixelSize()/2), num);
        }
        else
        {
            pos = -Ypos + centre.y;
            space = fontMetrics.width("0") + 5;
            painter.drawText(QPointF(-space, pos + graphSettings.graphFont.pixelSize()/2), "0");
        }

        if(space > largestWidth)
            largestWidth = space;

        Ypos += step;
    }

    if(leftMargin - additionalMargin - largestWidth > 8 || leftMargin - additionalMargin - largestWidth < 4)
    {
        leftMargin = largestWidth + additionalMargin + 6;
        update();
    }


}

void ImagePreview::drawAxes()
{
    //TODO: update this method

    pen.setWidth(1);
    pen.setColor(graphSettings.axesColor);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, false);



    if(graphRange.Ymin < 0 && graphRange.Ymax > 0)
        painter.drawLine(QPointF(0, centre.y), QPointF(graphRectScaled.width(), centre.y));
    if(graphRange.Xmin < 0 && graphRange.Xmax > 0)
        painter.drawLine(QPointF(centre.x, 0), QPointF(centre.x, graphRectScaled.height()));

    painter.drawRect(graphRectScaled);


}

void ImagePreview::updateCenterPosAndScaling()
{
    // TODO: update this method

    uniteY = double(graphRectScaled.height()) / (graphRange.Ymax - graphRange.Ymin);
    uniteX = double(graphRectScaled.width()) / (graphRange.Xmax - graphRange.Xmin);


    double rapport = uniteY / uniteX;

    if(information->isOrthonormal())
    {

    }
    centre.x = - graphView->viewRect().left() * uniteX;
    centre.y =  graphView->viewRect().top() * uniteY;
}

QImage* ImagePreview::drawImage()
{
    //TODO: update this method

    viewSettings = information->getViewSettings();

    QImage *image = new QImage(size(), QImage::Format_RGB32);
    image->fill(viewSettings.graph.backgroundColor.rgb());

    painter.begin(image);
    //trace du background

    pen.setColor(viewSettings.axes.x.color);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawRect(leftMargin, topMargin, graphRectScaled.width(), graphRectScaled.height());

    painter.translate(leftMargin, topMargin);

    updateCenterPosAndScaling();
    drawAxes();
    drawTicksAndNumbers();

    if(legendState)
        writeLegends();

    painter.setClipRect(0,0,graphRectScaled.width(),graphRectScaled.height());

    painter.translate(QPointF(centre.x, centre.y));

    if(recalculate)
    {
        funcValuesSaver->calculateAll(uniteX, uniteY, *graphView);
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
