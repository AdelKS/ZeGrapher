/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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


#include "GraphDraw/printpreview.h"

PrintPreview::PrintPreview(Informations *info) : ImagePreview(info)
{
    parametres.distanceEntrePoints = 0.125;
    viewType = PORTRAIT;
    graphHeightCm = 28.7;
    graphWidthCm = 20;
    hasScale = false;   
    xscale = yscale = 1;
    relativeXposCm = relativeYposCm = 0;
    moveType = NOTHING;

}

void PrintPreview::setPrinter(QPrinterInfo printInfo)
{
    printerInfo = printInfo;
}

void PrintPreview::setPDFname(QString pdf)
{
    PDFname = pdf;
}

void PrintPreview::print(int nbPages, bool colorType, bool printType, bool resType, int res)
{
    parametres = informations->getOptions();
    graphRange = informations->getRange();

    printer = new QPrinter(printerInfo);

    if(resType == CUSTOM_RESOLUTION)
        printer->setResolution(res);

    printer->setPaperSize(QPrinter::A4);
    printer->setFullPage(true);
    printer->setCopyCount(nbPages);   

    if(colorType == COLOR)
        printer->setColorMode(QPrinter::Color);
    else printer->setColorMode(QPrinter::GrayScale);

    if(viewType == PORTRAIT)
        printer->setOrientation(QPrinter::Portrait);
    else printer->setOrientation(QPrinter::Landscape);

    if(printType == PRINT_FILE)
        printer->setOutputFileName(PDFname);

    QRect sheet = printer->paperRect();
    graphHeight = sheet.height() * graphHeightCm / 29.7;
    graphWidth = sheet.width() * graphWidthCm / 21;

    QPoint graphOrigin;
    graphOrigin.setX((relativeXposCm + 0.5) * sheet.width() / 21);
    graphOrigin.setY((relativeYposCm + 0.5) * sheet.height() / 29.7);   

    painter.begin(printer);

    sheetWidth = sheet.width();
    sheetHeight = sheet.height();

    painter.translate(graphOrigin);

    assignGraphSize();
    determinerCentreEtUnites();
    paint();

    painter.end(); 

    delete printer;
    printer = NULL;
}

void PrintPreview::setScaleStatus(bool isActive)
{
    hasScale = isActive;
    repaint();
}

void PrintPreview::setxscale(double scale)
{
    xscale = scale;
    if(informations->isOrthonormal())
        yscale = xscale;
    repaint();
}

void PrintPreview::setyscale(double scale)
{
    yscale = scale;
    repaint();
}

void PrintPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    parametres = informations->getOptions();
    graphRange = informations->getRange();

    painter.begin(this);
    //trace du background
    drawSheet();
    drawGraph();

    painter.end();

    assignMouseRects();

}

void PrintPreview::assignMouseRects()
{
    QPointF point;

    point = graphRect.topLeft();
    point.setX(point.x() - 3);
    point.setY(point.y() - 3);
    topLeft.setTopLeft(point);

    point.setX(point.x() + 6);
    point.setY(point.y() + 6);
    topLeft.setBottomRight(point);

    point = graphRect.topRight();
    point.setX(point.x() - 3);
    point.setY(point.y() - 3);
    topRight.setTopLeft(point);

    point.setX(point.x() + 6);
    point.setY(point.y() + 6);
    topRight.setBottomRight(point);

    point = graphRect.bottomRight();
    point.setX(point.x() - 3);
    point.setY(point.y() - 3);
    bottomRight.setTopLeft(point);

    point.setX(point.x() + 6);
    point.setY(point.y() + 6);
    bottomRight.setBottomRight(point);

    point = graphRect.bottomLeft();
    point.setX(point.x() - 3);
    point.setY(point.y() - 3);
    bottomLeft.setTopLeft(point);

    point.setX(point.x() + 6);
    point.setY(point.y() + 6);
    bottomLeft.setBottomRight(point);

    top.setTopLeft(topLeft.topRight());
    top.setBottomRight(topRight.bottomLeft());

    bottom.setTopLeft(bottomLeft.topRight());
    bottom.setBottomRight(bottomRight.bottomLeft());

    left.setTopLeft(topLeft.bottomLeft());
    left.setBottomRight(bottomLeft.topRight());

    right.setTopLeft(topRight.bottomLeft());
    right.setBottomRight(bottomRight.topRight());
}


void PrintPreview::determinerCentreEtUnites()
{
    uniteY = graphHeight / (graphRange.Ymax - graphRange.Ymin);
    uniteX = graphWidth / (graphRange.Xmax - graphRange.Xmin);

    if(informations->isOrthonormal())
    {
        double rapport =  uniteY / uniteX;
        graphRange.Ymin *= rapport;
        graphRange.Ymax *= rapport;
        uniteY = uniteX;
        xscale = yscale;
    }
    if(hasScale)
    {
       double newxunit = xscale * graphWidth / graphWidthCm;
       double newyunit = yscale * graphHeight / graphHeightCm;

       double rapportx = uniteX / newxunit;
       double rapporty = uniteY / newyunit;

       graphRange.Xmax *= rapportx;
       graphRange.Xmin *= rapportx;
       graphRange.Ymax *= rapporty;
       graphRange.Ymin *= rapporty;

       uniteX = newxunit;
       uniteY = newyunit;
    }

    centre.x = - graphRange.Xmin * uniteX;
    centre.y =  graphRange.Ymax * uniteY;
}

void PrintPreview::drawSheet()
{
    painter.setBrush(QBrush(Qt::white));;

    double rapport;

    if(viewType == PORTRAIT)
    {
        rapport = (double)height() / (double)width();

        if(rapport < 1.4142)
        {
            sheetHeight = height() - 2;
            sheetWidth = sheetHeight / M_SQRT2;

            sheetRect.setTopLeft(QPointF((width() - sheetWidth)/2, 0));

        }
        else
        {
            sheetWidth = width() - 2;
            sheetHeight = sheetWidth * M_SQRT2;

            sheetRect.setTopLeft(QPointF(0, (height() - sheetHeight)/2));
        }
    }
    else
    {
        rapport = (double)width() / (double)height();

        if(rapport > 1.4142)
        {
            sheetHeight = height() - 2;
            sheetWidth = sheetHeight * M_SQRT2;

            sheetRect.setTopLeft(QPointF((width() - sheetWidth)/2, 0));
        }
        else
        {
            sheetWidth = width() - 2;
            sheetHeight = sheetWidth / M_SQRT2;

            sheetRect.setTopLeft(QPointF(0, (height() - sheetHeight)/2));
        }
    }

    sheetRect.setSize(QSizeF(sheetWidth, sheetHeight));

    graphRect.setHeight(sheetHeight * graphHeightCm / 29.7);
    graphRect.setWidth(sheetWidth * graphWidthCm / 21);

    QPointF point;
    if(viewType == PORTRAIT)
    {
        point.setX(sheetRect.topLeft().x() + (relativeXposCm + 0.5) * sheetWidth / 21);
        point.setY(sheetRect.topLeft().y() + (relativeYposCm + 0.5) * sheetHeight / 29.7);
    }
    else
    {
        point.setX(sheetRect.topLeft().x() + (relativeXposCm + 0.5) * sheetWidth / 29.7);
        point.setY(sheetRect.topLeft().y() + (relativeYposCm + 0.5) * sheetHeight / 21);
    }

     graphRect.setTopLeft(point);

    painter.drawRect(sheetRect);
}

void PrintPreview::drawGraph()
{
    assignGraphSize();

    painter.setBrush(Qt::NoBrush);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawRect(graphRect);

    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    painter.translate(graphRect.topLeft());

    determinerCentreEtUnites();
    paint();
}

void PrintPreview::setGraphHeight(double H)
{
    graphHeightCm = H;
    testGraphPosition();
    repaint();
}

void PrintPreview::setGraphWidth(double W)
{
    graphWidthCm = W;
    testGraphPosition();
    repaint();
}

void PrintPreview::setViewType(bool type)
{
    viewType = type;
    relativeXposCm = relativeYposCm = 0;
    if(viewType == PORTRAIT)
    {
       graphHeightCm = 28.7;
       graphWidthCm = 20;
    }
    else
    {
        graphHeightCm = 20;
        graphWidthCm = 28.7;
    }
    emit newGraphSize(graphHeightCm, graphWidthCm);
    repaint();
}

void PrintPreview::assignGraphSize()
{
    if(viewType == PORTRAIT)
    {
        graphRect.setWidth(graphWidthCm * sheetWidth / 21);
        graphRect.setHeight(graphHeightCm * sheetHeight / 29.7);
    }
    else
    {
        graphRect.setWidth(graphWidthCm * sheetWidth / 29.7);
        graphRect.setHeight(graphHeightCm * sheetHeight / 21);
    }

    graphWidth = graphRect.width()-(leftMargin+rightMargin);
    graphHeight = graphRect.height()-(topMargin+bottomMargin);
}

void PrintPreview::mousePressEvent(QMouseEvent *event)
{
    if(topLeft.contains(event->posF()))
        moveType = TOPLEFT_CORNER;
    else if(topRight.contains(event->posF()))
        moveType = TOPRIGHT_CORNER;
    else if(top.contains(event->posF()))
        moveType = TOP_SIDE;
    else if(bottomLeft.contains(event->posF()))
        moveType = BOTTOMLEFT_CORNER;
    else if(bottomRight.contains(event->posF()))
        moveType = BOTTOMRIGHT_CORNER;
    else if(bottom.contains(event->posF()))
        moveType = BOTTOM_SIDE;
    else if(left.contains(event->posF()))
        moveType = LEFT_SIDE;
    else if(right.contains(event->posF()))
        moveType = RIGHT_SIDE;
    else if(graphRect.contains(event->posF()))    
        moveType = ALL;    
    else moveType = NOTHING;

    if(moveType != NOTHING)
        lastMousePos = event->posF();
}

void PrintPreview::mouseMoveEvent(QMouseEvent *event)
{
    QRectF rect = sheetRect;
    rect.setTop(rect.top() - 5);
    rect.setRight(rect.right() - 5);
    rect.setLeft(rect.left() -  5);
    rect.setBottom(rect.bottom() - 5);
    if(moveType == NOTHING)
    {
        if(topLeft.contains(event->posF()) || bottomRight.contains(event->posF()))
            setCursor(Qt::SizeFDiagCursor);
        else if(topRight.contains(event->posF()) || bottomLeft.contains(event->posF()))
            setCursor(Qt::SizeBDiagCursor);
        else if(top.contains(event->posF()) || bottom.contains(event->posF()))
            setCursor(Qt::SizeVerCursor);
        else if(left.contains(event->posF()) || right.contains(event->posF()))
            setCursor(Qt::SizeHorCursor);
        else if(graphRect.contains(event->posF()))
            setCursor(Qt::SizeAllCursor);
        else setCursor(Qt::ArrowCursor);
    }
    else if(rect.contains(event->posF()))
    {
        double dx = event->posF().x() - lastMousePos.x(), dy = event->posF().y() - lastMousePos.y(), dxCm, dyCm;

        if(viewType == PORTRAIT)
        {
            dxCm = dx * 21 / sheetWidth;
            dyCm = dy * 29.7 / sheetHeight;
        }
        else
        {
            dxCm = dx * 29.7 / sheetWidth;
            dyCm = dy * 21 / sheetHeight;
        }


        switch(moveType)
        {
        case TOPLEFT_CORNER:
            relativeXposCm += dxCm;
            relativeYposCm += dyCm;
            graphWidthCm -= dxCm;
            graphHeightCm -= dyCm;
            break;
        case TOPRIGHT_CORNER:
            relativeYposCm += dyCm;
            graphWidthCm += dxCm;
            graphHeightCm -= dyCm;
            break;
        case BOTTOMLEFT_CORNER:
            relativeXposCm += dxCm;
            graphHeightCm += dyCm;
            graphWidthCm -= dxCm;
            break;
        case BOTTOMRIGHT_CORNER:
            graphWidthCm += dxCm;
            graphHeightCm += dyCm;
            break;
        case LEFT_SIDE:
            graphWidthCm -= dxCm;
            relativeXposCm += dxCm;
            break;
        case TOP_SIDE:
            graphHeightCm -= dyCm;
            relativeYposCm += dyCm;
            break;
        case RIGHT_SIDE:
            graphWidthCm += dxCm;
            break;
        case BOTTOM_SIDE:
            graphHeightCm += dyCm;
            break;
        case ALL:
            relativeXposCm += dxCm;
            relativeYposCm += dyCm;
            break;
        }

        testGraphPosition();
        repaint();



        lastMousePos = event->posF();
    }
}

void PrintPreview::testGraphPosition()
{
    if(relativeXposCm < 0)
        relativeXposCm = 0;
    if(relativeYposCm < 0)
        relativeYposCm = 0;
    if(viewType == PORTRAIT)
    {
        if(graphHeightCm > 28.7)
            graphHeightCm = 28.7;
        if(graphHeightCm < 10)
            graphHeightCm = 10;
        if(graphWidthCm > 20)
            graphWidthCm = 20;
        if(graphWidthCm < 10)
            graphWidthCm = 10;

        if(relativeXposCm + graphWidthCm > 20)
            relativeXposCm = 20 - graphWidthCm;
        if(relativeYposCm + graphHeightCm > 28.7)
            relativeYposCm = 28.7 - graphHeightCm;
    }
    else
    {
        if(graphHeightCm > 20)
            graphHeightCm = 20;
        if(graphHeightCm < 10)
            graphHeightCm = 10;
        if(graphWidthCm > 28.7)
            graphWidthCm = 28.7;
        if(graphWidthCm < 10)
            graphWidthCm = 10;

        if(relativeXposCm + graphWidthCm > 28.7)
            relativeXposCm = 28.7 - graphWidthCm;
        if(relativeYposCm + graphHeightCm > 20)
            relativeYposCm = 20 - graphHeightCm;
    }

    emit newGraphSize(graphHeightCm, graphWidthCm);
}

void PrintPreview::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
    moveType = NOTHING;
}

void PrintPreview::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}
