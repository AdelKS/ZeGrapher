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

ExportPreview::ExportPreview(QSizeF sheetSizeCm, QSize imageSizePixels, ExportType exportType, Information *info): ImagePreview(info)
{
    this->sheetSizeCm = sheetSizeCm;
    this->imageSizePx = imageSizePixels;
    this->exportType = exportType;

    initialise();
}

void ExportPreview::initialise()
{
    graphSettings.distanceBetweenPoints = information->getSettingsVals().distanceBetweenPoints;
    orientation = QPageLayout::Landscape;
    moveType = NOTHING;
    setMouseTracking(true);
    userScalingFactor = 1;
    sheetMarginCm = 0;
    imageMarginPx = 0;

    minRelSize = RELATIVE_MIN_SIZE;

    screenResolution = qGuiApp->primaryScreen()->physicalDotsPerInch();

    sheetFigureRectRelative.setHeight(1);
    sheetFigureRectRelative.setWidth(1);
    sheetFigureRectRelative.moveTopLeft(QPointF(0, 0));

    imageFigureRectRelative.setHeight(1);
    imageFigureRectRelative.setWidth(1);
    imageFigureRectRelative.moveTopLeft(QPointF(0, 0));
}

void ExportPreview::setSheetMarginCm(double sheetMarginCm)
{
    this->sheetMarginCm = sheetMarginCm;
}

void ExportPreview::setImageMarginPx(int imageMarginPx)
{
    this->imageMarginPx = imageMarginPx;
}

QSize ExportPreview::getTargetSheetSizePixels()
{
    return targetSheetSizePixels;
}

double ExportPreview::getMinFigureSize()
{
    return minRelSize;
}

void ExportPreview::setScale(double scalingFactor)
{
    userScalingFactor = scalingFactor;
    update();
}

void ExportPreview::exportPDF(QString fileName, SheetSizeType sizeType)
{
    graphSettings = information->getSettingsVals();
    graphRange = information->getRange();

    QPdfWriter *pdfWriter = new QPdfWriter(fileName);

    int targetResolution = int(screenResolution / userScalingFactor);

    pdfWriter->setResolution(targetResolution);   

    QPageLayout layout;
    layout.setUnits(QPageLayout::Millimeter);

    QSizeF sheetSizeMm(sheetSizeCm.width() * 10, sheetSizeCm.height() * 10);
    layout.setPageSize(QPageSize(sheetSizeMm, QPageSize::Millimeter, "", QPageSize::FuzzyOrientationMatch));

    if(sizeType == SheetSizeType::NORMALISED)
        layout.setOrientation(orientation);
    else layout.setOrientation(QPageLayout::Portrait);

    pdfWriter->setPageLayout(layout);

    painter.begin(pdfWriter);

    if(information->getSettingsVals().backgroundColor != QColor(Qt::white))
    {
        painter.setBrush(QBrush(information->getSettingsVals().backgroundColor));
        painter.drawRect(painter.viewport());
    }

    figureRectScaled = getFigureRectFromRelative(painter.viewport());

    painter.translate(figureRectScaled.topLeft());

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

    if(exportType == ExportType::VECTORIAL)
        targetRatio = sheetSizeCm.height() / sheetSizeCm.width();
    else targetRatio = double(imageSizePx.height()) / double(imageSizePx.width());

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
    double newZoom;

    if(exportType == ExportType::VECTORIAL)
    {
        QSizeF sheetSizeMm(sheetSizeCm.width() * 10, sheetSizeCm.height() * 10);

        QPageLayout layout(QPageSize(sheetSizeMm, QPageSize::Millimeter), orientation, QMarginsF(), QPageLayout::Millimeter);
        layout.setOrientation(orientation);
        targetSheetSizePixels = layout.fullRectPixels(int(screenResolution)).size();

        if(fabs(layout.fullRect().width() / layout.fullRect().height() - sheetSizeMm.width() / sheetSizeMm.height()) > 0.01)
            targetSheetSizePixels.transpose();

         newZoom = double(refSheetRect.width()) / double(targetSheetSizePixels.width());
    }
    else
    {
        newZoom = double(refSheetRect.width()) / double(imageSizePx.width());
    }

    if(fabs(newZoom - currentZoom) > 0.001)
        emit newZoomValue(newZoom);

    currentZoom = newZoom;

    double totalScaleFactor = currentZoom * userScalingFactor;


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

void ExportPreview::setSheetFigureSizeCm(QSizeF sizeCm)
{
    figureSizeCm = sizeCm;
    sheetFigureRectRelative.setWidth(figureSizeCm.width() / sheetSizeCm.width());
    sheetFigureRectRelative.setHeight(figureSizeCm.height() / sheetSizeCm.height());
    constrainCanvasRectRel();
}

void ExportPreview::setImageFigureSizePx(QSize sizePx)
{
    figureSizePx = sizePx;
    sheetFigureRectRelative.setWidth(double(figureSizePx.width()) / double(imageSizePx.width()));
    sheetFigureRectRelative.setHeight(double(figureSizePx.height()) / double(imageSizePx.height()));
    constrainCanvasRectRel();
}

void ExportPreview::setSheetSizeCm(QSizeF sizeCm)
{
    sheetSizeCm = sizeCm;
    figureSizeCm.setHeight(sheetFigureRectRelative.height() * sheetSizeCm.height());
    figureSizeCm.setWidth(sheetFigureRectRelative.width() * sheetSizeCm.width());

    emit newFigureSizeCm(figureSizeCm);
}

void ExportPreview::setImageSizePx(QSize sizePx)
{
    imageSizePx = sizePx;
    figureSizePx.setHeight(int(imageFigureRectRelative.height() * double(imageSizePx.height())));
    figureSizePx.setWidth(int(imageFigureRectRelative.width() * double(imageSizePx.width())));

    emit newFigureSizePx(figureSizePx);
}

void ExportPreview::setSheetOrientation(QPageLayout::Orientation type)
{
    if(type != orientation)
    {
         orientation = type;
         sheetFigureRectRelative = sheetFigureRectRelative.transposed();
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

        sheetFigureRectRelative.setTopLeft(pt);
        sheetFigureRectRelative.setWidth(double(figureRect.width()) / double(sheetRect.width()));
        sheetFigureRectRelative.setHeight(double(figureRect.height()) / double(sheetRect.height()));

        constrainCanvasRectRel();
        update();
    }

}

void ExportPreview::constrainCanvasRectRel()
{
    if(exportType == ExportType::VECTORIAL)
    {
        if(sheetFigureRectRelative.width() > 1)
            sheetFigureRectRelative.setWidth(1);
        if(sheetFigureRectRelative.height() > 1)
            sheetFigureRectRelative.setHeight(1);

        if(sheetFigureRectRelative.width() < minRelSize)
            sheetFigureRectRelative.setWidth(minRelSize);
        if(sheetFigureRectRelative.height() < minRelSize)
            sheetFigureRectRelative.setHeight(minRelSize);

        if(sheetFigureRectRelative.left() < 0)
            sheetFigureRectRelative.moveLeft(0);
        if(sheetFigureRectRelative.right() > 1)
            sheetFigureRectRelative.moveRight(1);

        if(sheetFigureRectRelative.bottom() > 1)
            sheetFigureRectRelative.moveBottom(1);
        if(sheetFigureRectRelative.top() < 0)
            sheetFigureRectRelative.moveTop(0);

        QSizeF oldFigureSizeCm = figureSizeCm;

        figureSizeCm.setWidth(sheetFigureRectRelative.width() * sheetSizeCm.width());
        figureSizeCm.setHeight(sheetFigureRectRelative.height() * sheetSizeCm.height());

        if(oldFigureSizeCm != figureSizeCm)
        {
            emit newFigureSizeCm(figureSizeCm);
        }
    }
    else
    {
        if(imageFigureRectRelative.width() > 1)
            imageFigureRectRelative.setWidth(1);
        if(imageFigureRectRelative.height() > 1)
            imageFigureRectRelative.setHeight(1);

        if(imageFigureRectRelative.width() < minRelSize)
            imageFigureRectRelative.setWidth(minRelSize);
        if(imageFigureRectRelative.height() < minRelSize)
            imageFigureRectRelative.setHeight(minRelSize);

        if(imageFigureRectRelative.left() < 0)
            imageFigureRectRelative.moveLeft(0);
        if(imageFigureRectRelative.right() > 1)
            imageFigureRectRelative.moveRight(1);

        if(imageFigureRectRelative.bottom() > 1)
            imageFigureRectRelative.moveBottom(1);
        if(imageFigureRectRelative.top() < 0)
            imageFigureRectRelative.moveTop(0);

        QSizeF oldFigureSizePx = figureSizePx;

        figureSizePx.setWidth(int(imageFigureRectRelative.width() * double(imageSizePx.width())));
        figureSizePx.setHeight(int(imageFigureRectRelative.height() * double(imageSizePx.height())));

        if(oldFigureSizePx != figureSizePx)
        {
            emit newFigureSizePx(figureSizePx);
        }
    }

}

QRect ExportPreview::getFigureRectFromRelative(QRect refSheetRect)
{
    QRect rect;
    QRectF relRect;
    QSizeF availableSize(refSheetRect.size());
    QPoint marginTopLeft;

    if(exportType == ExportType::VECTORIAL)
    {
        marginTopLeft.setX(int(sheetMarginCm / sheetSizeCm.width() * availableSize.width()));
        marginTopLeft.setY(int(sheetMarginCm / sheetSizeCm.height() * availableSize.height()));

        availableSize.setWidth(availableSize.width() - 2 * sheetMarginCm / sheetSizeCm.width() * availableSize.width());
        availableSize.setHeight(availableSize.height() - 2 * sheetMarginCm / sheetSizeCm.height() * availableSize.height());

        relRect = sheetFigureRectRelative;
    }
    else
    {
        marginTopLeft.setX(int(double(imageMarginPx) / double(imageSizePx.width()) * availableSize.width()));
        marginTopLeft.setY(int(double(imageMarginPx) / double(imageSizePx.height()) * availableSize.height()));

        availableSize.setWidth(availableSize.width() - 2 * double(imageMarginPx) / double(imageSizePx.width()) * availableSize.width());
        availableSize.setHeight(availableSize.height() - 2 * double(imageMarginPx) / double(imageSizePx.height()) * availableSize.height());

        relRect = imageFigureRectRelative;
    }


    rect.setWidth(int(relRect.width() * availableSize.width()));
    rect.setHeight(int(relRect.height() * availableSize.height()));
    rect.moveTopLeft(refSheetRect.topLeft() + marginTopLeft + QPoint(int(relRect.topLeft().x() * double(refSheetRect.width())),
                                                         int(relRect.topLeft().y() * double(refSheetRect.height()))));
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
