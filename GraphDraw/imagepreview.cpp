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


#include "GraphDraw/imagepreview.h"


ImagePreview::ImagePreview(Information *info) : GraphDraw(info)
{
    information = info;
    graphSettings.distanceBetweenPoints = 0.125;
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
    Q_UNUSED(event);

    graphSettings = information->getGraphSettings();
    graphView = information->getGraphView();

    painter.begin(this);    

    painter.setFont(information->getGraphSettings().graphFont);

    painter.setBrush(QBrush(graphSettings.backgroundColor));
    painter.drawRect(-1, -1, width()+1, height()+1);

    assignGraphSize();
    determinerCentreEtUnites();
    funcValuesSaver->calculateAll(uniteX, uniteY, graphView);
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

void ImagePreview::assignGraphSize()
{
    graphWidth = width()-(rightMargin+leftMargin);
    graphHeight = height()-(bottomMargin+topMargin);
}

void ImagePreview::paint()
{
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.translate(leftMargin, topMargin);

    pen.setColor(information->getGraphSettings().axesColor);
    painter.setPen(pen);

    placerGraduations();
    drawAxes();

    painter.drawRect(0, 0, graphWidth, graphHeight);

    if(legendState)
        writeLegends();

    painter.setClipRect(0,0,graphWidth,graphHeight);

    painter.translate(QPointF(centre.x, centre.y));

    funcValuesSaver->calculateAll(uniteX, uniteY, graphView);
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
        startDrawPoint.setX((graphWidth - xLegendWidth)/2);
        startDrawPoint.setY(graphHeight + bottomMargin - 10);

        painter.drawText(startDrawPoint, xLegend);
    }
    if(!yLegend.isEmpty())
    {
        painter.rotate(-90);
        int yLegendWidth = painter.fontMetrics().width(yLegend);
        int yLegendHeight = legendFontSize + 6;

        QPoint startDrawPoint;
        startDrawPoint.setX(- (graphHeight - (graphHeight - yLegendWidth)/2));
        startDrawPoint.setY(-leftMargin + yLegendHeight);

        painter.drawText(startDrawPoint, yLegend);

        painter.rotate(90);
    }
}

void ImagePreview::placerGraduations()
{
    double fontSize = information->getGraphSettings().graphFont.pixelSize();
    double prec = numPrec;

    font.setPixelSize(fontSize);
    font.setItalic(false);
    font.setBold(false);
    font.setUnderline(false);    
    pen.setWidth(1);
    painter.setFont(font);
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
                painter.drawLine(QPointF(Xpos + centre.x, 0), QPointF(Xpos + centre.x, graphHeight));
            }
            pen.setColor(graphSettings.axesColor);
            pen.setWidth(1);            
            painter.setPen(pen);

            pos = Xpos + centre.x;

            painter.drawLine(QPointF(pos, 4), QPointF(pos, 0));
            painter.drawLine(QPointF(pos, graphHeight-4), QPointF(pos, graphHeight));
            num = QString::number(Xpos/uniteX, 'g', prec);
            space = fontMetrics.width(num);
            painter.drawText(QPointF(pos - space/2, graphHeight+15), num);
        }
        else
        {
            pos = Xpos + centre.x;
            space = fontMetrics.width("0");
            painter.drawText(QPointF(pos - space/2, graphHeight+15), "0");
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
                painter.drawLine(QPointF(0, -Ypos + centre.y), QPointF(graphWidth, -Ypos + centre.y));
            }

            pen.setColor(graphSettings.axesColor);
            pen.setWidth(1);
            painter.setPen(pen);

            pos = -Ypos + centre.y;

            painter.drawLine(QPointF(4, pos), QPointF(0, pos));
            painter.drawLine(QPointF(graphWidth - 4, pos), QPointF(graphWidth, pos));

            num = QString::number(Ypos/uniteY, 'g', prec);
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
    pen.setWidth(1);
    pen.setColor(graphSettings.axesColor);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, false);

    if(graphView.Ymin < 0 && graphView.Ymax > 0)
        painter.drawLine(QPointF(0, centre.y), QPointF(graphWidth, centre.y));
    if(graphView.Xmin < 0 && graphView.Xmax > 0)
        painter.drawLine(QPointF(centre.x, 0), QPointF(centre.x, graphHeight));

}

void ImagePreview::determinerCentreEtUnites()
{
    uniteY = graphHeight / (graphView.Ymax - graphView.Ymin);
    uniteX = graphWidth / (graphView.Xmax - graphView.Xmin);

    double rapport = uniteY / uniteX;
    if(information->isOrthonormal())
    {
        graphView.Ymin *= rapport;
        graphView.Ymax *= rapport;
        uniteY = uniteX;
    }


    centre.x = - graphView.Xmin * uniteX;
    centre.y =  graphView.Ymax * uniteY;
}

QImage* ImagePreview::drawImage()
{
    graphSettings = information->getGraphSettings();
    graphView = information->getGraphView();

    QImage *image = new QImage(size(), QImage::Format_RGB32);
    image->fill(graphSettings.backgroundColor.rgb());

    painter.begin(image);
    //trace du background  

    pen.setColor(graphSettings.axesColor);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.drawRect(leftMargin, topMargin, graphWidth, graphHeight);

    painter.translate(leftMargin, topMargin);

    determinerCentreEtUnites();
    drawAxes();
    placerGraduations();

    if(legendState)
        writeLegends();

    painter.setClipRect(0,0,graphWidth,graphHeight);

    painter.translate(QPointF(centre.x, centre.y));

    if(recalculate)
    {
        funcValuesSaver->calculateAll(uniteX, uniteY, graphView);
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
