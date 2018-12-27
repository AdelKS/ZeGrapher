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


#include "GraphDraw/exportpreview.h"

ExportPreview::ExportPreview(Information *info) : ImagePreview(info)
{
    parameters.distanceBetweenPoints = info->getSettingsVals().distanceBetweenPoints;
    orientation = QPageLayout::Landscape;
    moveType = NOTHING;
    setMouseTracking(true);
    userScalingFactor = 1;

    relativeSheetMargin = 0.05;
    canvasRectRelative.setWidth(1 - relativeSheetMargin);
    canvasRectRelative.setHeight(1 - relativeSheetMargin);
    canvasRectRelative.setTopLeft(QPointF(relativeSheetMargin, relativeSheetMargin));

    screenResolution = qGuiApp->primaryScreen()->physicalDotsPerInch();

}

void ExportPreview::setScale(double scalingFactor)
{
    userScalingFactor = scalingFactor;
    update();
}

void ExportPreview::exportPDF(QString fileName)
{
    parameters = information->getSettingsVals();
    graphRange = information->getRange();

    QPdfWriter *pdfWriter = new QPdfWriter(fileName);

    int targetResolution = int(screenResolution / userScalingFactor);

    pdfWriter->setResolution(targetResolution);

    QPageLayout layout;
    layout.setPageSize(QPageSize(QPageSize::A4));
    layout.setOrientation(orientation);

    sheetSizeCm.setWidth(sheetRect.width() * 0.1);
    sheetSizeCm.setHeight(sheetRect.height() * 0.1);

    pdfWriter->setPageLayout(layout);

    QRect exportSheetRect = pdfWriter->pageLayout().fullRectPixels(targetResolution);
    drawableRect.setHeight(int(double(sheetRect.height()) * canvasSizeCm.height() / sheetSizeCm.height()));
    drawableRect.setWidth(int(double(sheetRect.width()) * canvasSizeCm.width() / sheetSizeCm.width()));

    QPoint graphOrigin;
    graphOrigin.setX(int( double(sheetRect.width()) / sheetSizeCm.width()));
    graphOrigin.setY(int( double(sheetRect.height()) / sheetSizeCm.height()));

    painter.begin(pdfWriter);

    if(information->getSettingsVals().backgroundColor != QColor(Qt::white))
    {
        painter.setBrush(QBrush(information->getSettingsVals().backgroundColor));
        painter.drawRect(painter.viewport());
    }

    painter.translate(graphOrigin);

    assignGraphSize();
    determinerCentreEtUnites();
    paint();

    painter.end();

    delete pdfWriter;
}


void ExportPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    parameters = information->getSettingsVals();
    graphRange = information->getRange();

    painter.begin(this);
    //trace du background

    painter.setFont(information->getSettingsVals().graphFont);

    drawSheet();
    drawGraph();

    painter.end();

    assignMouseRects();

}

void ExportPreview::assignMouseRects()
{
    QPointF topLeftTranslation;
    topLeftTranslation.setX(-8);
    topLeftTranslation.setY(-8);

    QPointF bottomRightTranslation;
    bottomRightTranslation.setX(8);
    bottomRightTranslation.setY(8);


    topLeft.setTopLeft(drawableRect.topLeft() + topLeftTranslation);
    topLeft.setBottomRight(drawableRect.topLeft() + bottomRightTranslation);

    topRight.setTopLeft(drawableRect.topRight() + topLeftTranslation);
    topRight.setBottomRight(drawableRect.topRight() + bottomRightTranslation);

    bottomLeft.setTopLeft(drawableRect.bottomLeft() + topLeftTranslation);
    bottomLeft.setBottomRight(drawableRect.bottomLeft() + bottomRightTranslation);

    bottomRight.setTopLeft(drawableRect.bottomRight() + topLeftTranslation);
    bottomRight.setBottomRight(drawableRect.bottomRight() + bottomRightTranslation);

    top.setTopLeft(topLeft.topRight());
    top.setBottomRight(topRight.bottomLeft());

    bottom.setTopLeft(bottomLeft.topRight());
    bottom.setBottomRight(bottomRight.bottomLeft());

    left.setTopLeft(topLeft.bottomLeft());
    left.setBottomRight(bottomLeft.topRight());

    right.setTopLeft(topRight.bottomLeft());
    right.setBottomRight(bottomRight.topRight());
}

void ExportPreview::drawSheet()
{
    painter.setBrush(QBrush(information->getSettingsVals().backgroundColor));;

    double ratio, targetRatio;

    ratio = double(height()) / double(width());
    targetRatio = sheetSizeCm.height() / sheetSizeCm.width();

    if(ratio <= targetRatio)
    {
        sheetRect.setHeight(height());
        sheetRect.setWidth(int(double(height()) / targetRatio));

        sheetRect.setTopLeft(QPoint((width() - sheetRect.width())/2, 0));
    }
    else
    {
        sheetRect.setHeight(int(double(width()) * targetRatio));
        sheetRect.setWidth(width());

        sheetRect.setTopLeft(QPoint(0, (height() - sheetRect.height())/2));
    }

    updateDrawableRectFromRel();

    painter.drawRect(sheetRect);
}

void ExportPreview::drawGraph()
{
    assignGraphSize();

    painter.setBrush(Qt::NoBrush);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    pen.setColor(Qt::red);//information->getSettingsVals().axesColor);
    painter.setPen(pen);
    painter.drawRect(drawableRect);

    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    determinerCentreEtUnites();

    painter.translate(drawableRect.topLeft());

    double widthSheetScale = double(sheetRect.width()) / double(painter.viewport().width());
    double heightSheetScale = double(sheetRect.height()) / double(painter.viewport().height());

    painter.scale(widthSheetScale * userScalingFactor, heightSheetScale * userScalingFactor);


    paint();
}

void ExportPreview::setCanvasSizeCm(QSizeF sizeCm)
{
    canvasSizeCm = sizeCm;
    canvasRectRelative.setWidth(int( double(canvasSizeCm.width()) / double(sheetSizeCm.width())));
    canvasRectRelative.setHeight(int( double(canvasSizeCm.height()) / double(sheetSizeCm.height())));
    constrainCanvasRectRel();

    update();
}

void ExportPreview::setSheetSizeCm(QSizeF sizeCm)
{
    sheetSizeCm = sizeCm;
    update();
}

void ExportPreview::setOrientation(QPageLayout::Orientation type)
{
    if(type != orientation)
    {
         orientation = type;
         canvasRectRelative = canvasRectRelative.transposed();
    }

    repaint();
}

void ExportPreview::mousePressEvent(QMouseEvent *event)
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
    else if(drawableRect.contains(event->pos()))
        moveType = ALL;
    else moveType = NOTHING;

    if(moveType != NOTHING)
        lastMousePos = event->pos();
}

void ExportPreview::mouseMoveEvent(QMouseEvent *event)
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
        else if(drawableRect.contains(event->pos()))
            setCursor(Qt::SizeAllCursor);
        else setCursor(Qt::ArrowCursor);
    }
    else
    {
        QPoint dr = event->pos() - lastMousePos;

        switch(moveType)
        {
        case TOPLEFT_CORNER:
            drawableRect.setTopLeft(drawableRect.topLeft() + dr);
            break;
        case TOPRIGHT_CORNER:
            drawableRect.setTopRight(drawableRect.topRight() + dr);
            break;
        case BOTTOMLEFT_CORNER:
            drawableRect.setBottomLeft(drawableRect.bottomLeft() + dr);
            break;
        case BOTTOMRIGHT_CORNER:
            drawableRect.setBottomRight(drawableRect.bottomRight() + dr);
            break;
        case LEFT_SIDE:
            drawableRect.setLeft(drawableRect.left() + dr.x());
            break;
        case TOP_SIDE:
            drawableRect.setTop(drawableRect.top() + dr.y());
            break;
        case RIGHT_SIDE:
            drawableRect.setRight(drawableRect.right() + dr.x());
            break;
        case BOTTOM_SIDE:
            drawableRect.setBottom(drawableRect.bottom() + dr.y());
            break;
        case ALL:
            drawableRect.translate(dr);
            break;
        case NOTHING:
            break;
        }

        QPointF pt = drawableRect.topLeft() - sheetRect.topLeft();
        pt.setX(pt.x() / double(sheetRect.width()));
        pt.setY(pt.y() / double(sheetRect.height()));

        canvasRectRelative.setTopLeft(pt);
        canvasRectRelative.setWidth(double(drawableRect.width()) / double(sheetRect.width()));
        canvasRectRelative.setHeight(double(drawableRect.height()) / double(sheetRect.height()));

        constrainCanvasRectRel();
        updateDrawableRectFromRel();
        repaint();
    }

    lastMousePos = event->pos();
}

void ExportPreview::constrainCanvasRectRel()
{
    if(canvasRectRelative.width() > 1 - relativeSheetMargin)
        canvasRectRelative.setWidth(1 - relativeSheetMargin);
    if(canvasRectRelative.height() > 1 - relativeSheetMargin)
        canvasRectRelative.setHeight(1 - relativeSheetMargin);

    QSizeF updatedGraphSizeCm;
    updatedGraphSizeCm.setHeight(canvasRectRelative.width() * sheetSizeCm.width());
    updatedGraphSizeCm.setWidth(canvasRectRelative.height() * sheetSizeCm.height());

    if(canvasSizeCm != updatedGraphSizeCm)
    {
        canvasSizeCm = updatedGraphSizeCm;
        emit newCanvasSizeCm(canvasSizeCm);
    }

    QPointF topLeft = canvasRectRelative.topLeft();
    if(topLeft.x() + canvasRectRelative.width() > 1 - relativeSheetMargin/2)
        topLeft.setX(1 - relativeSheetMargin/2 - canvasRectRelative.width());
    if(topLeft.y() + canvasRectRelative.height() > 1 - relativeSheetMargin/2)
        topLeft.setY(1 - relativeSheetMargin/2 - canvasRectRelative.height());

    canvasRectRelative.setTopLeft(topLeft);
}

void ExportPreview::updateDrawableRectFromRel()
{
    drawableRect.setWidth(int(canvasRectRelative.width() * double(sheetRect.width())));
    drawableRect.setHeight(int(canvasRectRelative.height() * double(sheetRect.height())));
    drawableRect.setTopLeft(sheetRect.topLeft() + QPoint(int(canvasRectRelative.topLeft().x() * double(sheetRect.width())),
                                                         int(canvasRectRelative.topLeft().y() * double(sheetRect.height()))));
}

void ExportPreview::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
    moveType = NOTHING;
}

void ExportPreview::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}
