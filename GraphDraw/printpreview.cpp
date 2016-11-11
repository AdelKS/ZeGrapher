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




#include "GraphDraw/printpreview.h"

PrintPreview::PrintPreview(Information *info) : ImagePreview(info)
{
    graphSettings.distanceBetweenPoints = 0.125;
    viewType = PORTRAIT;
    graphHeightCm = 28.7;
    graphWidthCm = 20;  
    relativeXposCm = relativeYposCm = 0;
    moveType = NOTHING;
    setMouseTracking(true);

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
    graphSettings = information->getGraphSettings();
    graphRange = information->getGraphRange();

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

    if(printType == PRINT_FILE)
    {
        painter.setBrush(QBrush(information->getGraphSettings().backgroundColor));
        painter.drawRect(printer->paperRect());
    }


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

void PrintPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    graphSettings = information->getGraphSettings();
    graphRange = information->getGraphRange();

    painter.begin(this);
    //trace du background

    painter.setFont(information->getGraphSettings().graphFont);

    drawSheet();
    drawGraph();

    painter.end();

    assignMouseRects();

}

void PrintPreview::assignMouseRects()
{
    QPointF topLeftTranslation;
    topLeftTranslation.setX(-8);
    topLeftTranslation.setY(-8);

    QPointF bottomRightTranslation;
    bottomRightTranslation.setX(8);
    bottomRightTranslation.setY(8);


    topLeft.setTopLeft(graphRect.topLeft() + topLeftTranslation);
    topLeft.setBottomRight(graphRect.topLeft() + bottomRightTranslation);

    topRight.setTopLeft(graphRect.topRight() + topLeftTranslation);
    topRight.setBottomRight(graphRect.topRight() + bottomRightTranslation);

    bottomLeft.setTopLeft(graphRect.bottomLeft() + topLeftTranslation);
    bottomLeft.setBottomRight(graphRect.bottomLeft() + bottomRightTranslation);

    bottomRight.setTopLeft(graphRect.bottomRight() + topLeftTranslation);
    bottomRight.setBottomRight(graphRect.bottomRight() + bottomRightTranslation);

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

    if(information->getGraphSettings().view.viewType == ScaleType::LINEAR_ORTHONORMAL)
    {
        double rapport =  uniteY / uniteX;
        graphRange.Ymin *= rapport;
        graphRange.Ymax *= rapport;
        uniteY = uniteX;
    }   

    centre.x = - graphRange.Xmin * uniteX;
    centre.y =  graphRange.Ymax * uniteY;
}

void PrintPreview::drawSheet()
{
    painter.setBrush(QBrush(information->getGraphSettings().backgroundColor));;

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
    pen.setColor(information->getGraphSettings().axesColor);
    painter.setPen(pen);
    painter.drawRect(graphRect);

//    painter.drawRect(topLeft);
//    painter.drawRect(topRight);
//    painter.drawRect(bottomLeft);
//    painter.drawRect(bottomRight);
//    painter.drawRect(top);
//    painter.drawRect(right);
//    painter.drawRect(left);
//    painter.drawRect(bottom);

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
    if(topLeft.contains(event->pos()))
        moveType = TOPLEFT_CORNER;
    else if(topRight.contains(event->pos()))
        moveType = TOPRIGHT_CORNER;
    else if(top.contains(event->pos()))
        moveType = TOP_SIDE;
    else if(bottomLeft.contains(event->pos()))
        moveType = BOTTOMLEFT_CORNER;
    else if(bottomRight.contains(event->pos()))
        moveType = BOTTOMRIGHT_CORNER;
    else if(bottom.contains(event->pos()))
        moveType = BOTTOM_SIDE;
    else if(left.contains(event->pos()))
        moveType = LEFT_SIDE;
    else if(right.contains(event->pos()))
        moveType = RIGHT_SIDE;
    else if(graphRect.contains(event->pos()))
        moveType = ALL;    
    else moveType = NOTHING;

    if(moveType != NOTHING)
        lastMousePos = event->pos();
}

void PrintPreview::mouseMoveEvent(QMouseEvent *event)
{

    if(moveType == NOTHING)
    {
        if(topLeft.contains(event->pos()) || bottomRight.contains(event->pos()))
            setCursor(Qt::SizeFDiagCursor);
        else if(topRight.contains(event->pos()) || bottomLeft.contains(event->pos()))
            setCursor(Qt::SizeBDiagCursor);
        else if(top.contains(event->pos()) || bottom.contains(event->pos()))
            setCursor(Qt::SizeVerCursor);
        else if(left.contains(event->pos()) || right.contains(event->pos()))
            setCursor(Qt::SizeHorCursor);
        else if(graphRect.contains(event->pos()))
            setCursor(Qt::SizeAllCursor);
        else setCursor(Qt::ArrowCursor);
    }

    double dx = event->pos().x() - lastMousePos.x(), dy = event->pos().y() - lastMousePos.y(), dxCm, dyCm;

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



    lastMousePos = event->pos();

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
