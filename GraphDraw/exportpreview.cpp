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

ExportPreview::ExportPreview(QSizeF sheetSizeInCm, Information *info) : ImagePreview(info)
{
    sheetSizeCm = sheetSizeInCm;
    relativeSheetMinMargin = RELATIVE_MIN_MARGIN;

    setMaximalCanvas();

    graphSettings.distanceBetweenPoints = info->getSettingsVals().distanceBetweenPoints;
    orientation = QPageLayout::Landscape;
    moveType = NOTHING;
    setMouseTracking(true);
    userScalingFactor = 1;


    minRelSize = RELATIVE_MIN_SIZE;

    screenResolution = qGuiApp->primaryScreen()->physicalDotsPerInch();

}

double ExportPreview::getMinMargin()
{
    return relativeSheetMinMargin;
}

double ExportPreview::getMinFigureSize()
{
    return minRelSize;
}

void ExportPreview::setMaximalCanvas()
{
    double widthMinMargin, heightMinMargin;
    if(sheetSizeCm.width() < sheetSizeCm.height())
    {
        widthMinMargin = relativeSheetMinMargin;
        heightMinMargin = relativeSheetMinMargin * sheetSizeCm.width() / sheetSizeCm.height();
    }
    else
    {
        widthMinMargin = relativeSheetMinMargin * sheetSizeCm.height() / sheetSizeCm.width();
        heightMinMargin = relativeSheetMinMargin;
    }

    figureRectRelative.setWidth(1 - widthMinMargin);
    figureRectRelative.setHeight(1 - heightMinMargin);
    figureRectRelative.moveTop(heightMinMargin/2);
    figureRectRelative.moveLeft(widthMinMargin/2);
}

void ExportPreview::setScale(double scalingFactor)
{
    userScalingFactor = scalingFactor;
    update();
}

void ExportPreview::exportPDF(QString fileName)
{
    graphSettings = information->getSettingsVals();
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
    figureRectScaled.setHeight(int(double(sheetRect.height()) * figureSizeCm.height() / sheetSizeCm.height()));
    figureRectScaled.setWidth(int(double(sheetRect.width()) * figureSizeCm.width() / sheetSizeCm.width()));

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

    updateGraphRect();
    updateCenterPosAndScaling();
    paint();

    painter.end();

    delete pdfWriter;
}


void ExportPreview::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    graphSettings = information->getSettingsVals();
    graphRange = information->getRange();

    painter.begin(this);    

    drawSheet();
    drawFigureRect();

    assignMouseRects();

    drawGraph();

    painter.end();
}

void ExportPreview::assignMouseRects()
{
    QPoint topLeftTranslation;
    topLeftTranslation.setX(-8);
    topLeftTranslation.setY(-8);

    QPoint bottomRightTranslation;
    bottomRightTranslation.setX(8);
    bottomRightTranslation.setY(8);


    topLeft.setTopLeft(figureRect.topLeft() + topLeftTranslation);
    topLeft.setBottomRight(figureRect.topLeft() + bottomRightTranslation);

    topRight.setTopLeft(figureRect.topRight() + topLeftTranslation);
    topRight.setBottomRight(figureRect.topRight() + bottomRightTranslation);

    bottomLeft.setTopLeft(figureRect.bottomLeft() + topLeftTranslation);
    bottomLeft.setBottomRight(figureRect.bottomLeft() + bottomRightTranslation);

    bottomRight.setTopLeft(figureRect.bottomRight() + topLeftTranslation);
    bottomRight.setBottomRight(figureRect.bottomRight() + bottomRightTranslation);

    top.setTopLeft(topLeft.topRight());
    top.setBottomRight(topRight.bottomLeft());

    bottom.setTopLeft(bottomLeft.topRight());
    bottom.setBottomRight(bottomRight.bottomLeft());

    left.setTopLeft(topLeft.bottomLeft());
    left.setBottomRight(bottomLeft.topRight());

    right.setTopLeft(topRight.bottomLeft());
    right.setBottomRight(bottomRight.topRight());
}

QRect ExportPreview::sheetRectFromViewRect(QRect viewRect)
{
    double ratio, targetRatio;

    ratio = double(viewRect.height()) / double(viewRect.width());
    targetRatio = sheetSizeCm.height() / sheetSizeCm.width();

    QRect rect;

    if(ratio <= targetRatio)
    {
        rect.setHeight(viewRect.height());
        rect.setWidth(int(double(viewRect.height()) / targetRatio));

        rect.moveTopLeft(QPoint((viewRect.width() - rect.width())/2, 0));
    }
    else
    {
        rect.setHeight(int(double(viewRect.width()) * targetRatio));
        rect.setWidth(viewRect.width());

        rect.moveTopLeft(QPoint(0, (viewRect.height() - rect.height())/2));
    }

    return rect;
}

void ExportPreview::drawSheet()
{ // draws the sheet on an untransformed view
    painter.setBrush(QBrush(information->getSettingsVals().backgroundColor));;

    sheetRect = sheetRectFromViewRect(painter.viewport());

    painter.drawRect(sheetRect);   
}

void ExportPreview::drawFigureRect()
{
    figureRect = getFigureRectFromRelative(sheetRect);

//    qDebug() << "Sheet rect: " << sheetRect;
//    qDebug() << "Figure rect: " << figureRect;
//    qDebug() << "Figure rect relative: " << figureRectRelative;
//    qDebug() << "---------------------------------";

    painter.setBrush(Qt::NoBrush);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    pen.setColor(information->getSettingsVals().axesColor);
    painter.setPen(pen);
    painter.drawRect(figureRect);

    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
}

void ExportPreview::scaleView(QRect refSheetRect)
{
    QSizeF sheetSizeMm(sheetSizeCm.width() * 10, sheetSizeCm.height() * 10);

    QPageLayout layout(QPageSize(sheetSizeMm, QPageSize::Millimeter), orientation, QMarginsF(), QPageLayout::Millimeter);
    QRect targetSheetRect = layout.fullRectPixels(int(screenResolution / userScalingFactor));

    double totalScaleFactor;
    totalScaleFactor = double(refSheetRect.width()) / double(targetSheetRect.width());

    painter.scale(totalScaleFactor, totalScaleFactor);
}

void ExportPreview::drawGraph()
{
    scaleView(sheetRect);

    sheetRectScaled = painter.worldTransform().inverted().mapRect(sheetRect);

    figureRectScaled = getFigureRectFromRelative(sheetRectScaled);

    painter.translate(figureRectScaled.topLeft());

    paint();
}

void ExportPreview::setFigureSizeCm(QSizeF sizeCm)
{
    figureSizeCm = sizeCm;
    figureRectRelative.setWidth(figureSizeCm.width() / sheetSizeCm.width());
    figureRectRelative.setHeight(figureSizeCm.height() / sheetSizeCm.height());
    constrainCanvasRectRel();
}

void ExportPreview::setSheetSizeCm(QSizeF sizeCm)
{
    sheetSizeCm = sizeCm;
    figureSizeCm.setHeight(figureRectRelative.height() * sheetSizeCm.height());
    figureSizeCm.setWidth(figureRectRelative.width() * sheetSizeCm.width());

    emit newFigureSizeCm(figureSizeCm);
}

void ExportPreview::setOrientation(QPageLayout::Orientation type)
{
    if(type != orientation)
    {
         orientation = type;
         figureRectRelative = figureRectRelative.transposed();
    }
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
    else if(figureRect.contains(event->pos()))
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
        else if(figureRect.contains(event->pos()))
            setCursor(Qt::SizeAllCursor);
        else setCursor(Qt::ArrowCursor);

        lastMousePos = event->pos();
    }
    else
    {
        QPoint dr = event->pos() - lastMousePos;
        lastMousePos = event->pos();

        switch(moveType)
        {
        case TOPLEFT_CORNER:
            figureRect.setTopLeft(figureRect.topLeft() + dr);
            break;
        case TOPRIGHT_CORNER:
            figureRect.setTopRight(figureRect.topRight() + dr);
            break;
        case BOTTOMLEFT_CORNER:
            figureRect.setBottomLeft(figureRect.bottomLeft() + dr);
            break;
        case BOTTOMRIGHT_CORNER:
            figureRect.setBottomRight(figureRect.bottomRight() + dr);
            break;
        case LEFT_SIDE:
            figureRect.setLeft(figureRect.left() + dr.x());
            break;
        case TOP_SIDE:
            figureRect.setTop(figureRect.top() + dr.y());
            break;
        case RIGHT_SIDE:
            figureRect.setRight(figureRect.right() + dr.x());
            break;
        case BOTTOM_SIDE:
            figureRect.setBottom(figureRect.bottom() + dr.y());
            break;
        case ALL:
            figureRect.translate(dr);
            break;
        case NOTHING:
            break;
        }

        QPointF pt = figureRect.topLeft() - sheetRect.topLeft();
        pt.setX(pt.x() / double(sheetRect.width()));
        pt.setY(pt.y() / double(sheetRect.height()));

        figureRectRelative.setTopLeft(pt);
        figureRectRelative.setWidth(double(figureRect.width()) / double(sheetRect.width()));
        figureRectRelative.setHeight(double(figureRect.height()) / double(sheetRect.height()));

        constrainCanvasRectRel();
        update();
    }

}

void ExportPreview::constrainCanvasRectRel()
{
    double widthMinMargin, heightMinMargin;
    if(sheetSizeCm.width() < sheetSizeCm.height())
    {
        widthMinMargin = relativeSheetMinMargin;
        heightMinMargin = relativeSheetMinMargin * sheetSizeCm.width() / sheetSizeCm.height();
    }
    else
    {
        widthMinMargin = relativeSheetMinMargin * sheetSizeCm.height() / sheetSizeCm.width();
        heightMinMargin = relativeSheetMinMargin;
    }   

    if(figureRectRelative.width() > 1 - widthMinMargin)
        figureRectRelative.setWidth(1 - widthMinMargin);
    if(figureRectRelative.height() > 1 - heightMinMargin)
        figureRectRelative.setHeight(1 - heightMinMargin);

    if(figureRectRelative.width() < minRelSize)
        figureRectRelative.setWidth(minRelSize);
    if(figureRectRelative.height() < minRelSize)
        figureRectRelative.setHeight(minRelSize);

    if(figureRectRelative.left() < widthMinMargin/2)
        figureRectRelative.moveLeft(widthMinMargin/2);
    if(figureRectRelative.right() > 1 - widthMinMargin/2)
        figureRectRelative.moveRight(1 - widthMinMargin/2);

    if(figureRectRelative.bottom() > 1 - heightMinMargin/2)
        figureRectRelative.moveBottom(1 - heightMinMargin/2);
    if(figureRectRelative.top() < heightMinMargin/2)
        figureRectRelative.moveTop(heightMinMargin/2);

    QSizeF oldFigureSizeCm = figureSizeCm;

    figureSizeCm.setWidth(figureRectRelative.width() * sheetSizeCm.width());
    figureSizeCm.setHeight(figureRectRelative.height() * sheetSizeCm.height());

    if(oldFigureSizeCm != figureSizeCm)
    {
        emit newFigureSizeCm(figureSizeCm);
    }
}

QRect ExportPreview::getFigureRectFromRelative(QRect refSheetRect)
{
    QRect rect;
    rect.setWidth(int(figureRectRelative.width() * double(refSheetRect.width())));
    rect.setHeight(int(figureRectRelative.height() * double(refSheetRect.height())));
    rect.moveTopLeft(refSheetRect.topLeft() + QPoint(int(figureRectRelative.topLeft().x() * double(refSheetRect.width())),
                                                         int(figureRectRelative.topLeft().y() * double(refSheetRect.height()))));
    return rect;
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
