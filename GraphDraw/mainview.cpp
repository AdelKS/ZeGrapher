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


#include "GraphDraw/mainview.h"

MainView::MainView(Information *info): BaseGraphDraw(info)
{
    orientation = QPageLayout::Landscape;
    moveType = NOTHING;
    setMouseTracking(true);
    sizeSettings.scalingFactor = 1;

    minRelSize = RELATIVE_MIN_SIZE;

    screenDPI = qGuiApp->primaryScreen()->physicalDotsPerInch();

    relFigRect.setHeight(1);
    relFigRect.setWidth(1);
    relFigRect.moveTopLeft(QPointF(0, 0));

    connect(information, SIGNAL(sizeSettingsChanged()), this, SLOT(onSizeSettingsChange()));
}

void MainView::onSizeUnitChange()
{
    updateTargetSupportSizePx();
}

void MainView::updateTargetSupportSizePx()
{
    if(sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        // TODO: determine orientation from the sheet size: whether or not sizeSettings.cmSheetSize height > width
        QSizeF sheetSizeMm(sizeSettings.cmSheetSize.width() * 10, sizeSettings.cmSheetSize.height() * 10);
        QPageLayout layout(QPageSize(sheetSizeMm, QPageSize::Millimeter), orientation, QMarginsF(), QPageLayout::Millimeter);
        layout.setOrientation(orientation);
        targetSupportSizePixels = layout.fullRectPixels(int(screenDPI)).size();

        if(fabs(layout.fullRect().width() / layout.fullRect().height() - sheetSizeMm.width() / sheetSizeMm.height()) > 0.01)
            targetSupportSizePixels.transpose();
    }
    else
    {
        targetSupportSizePixels = sizeSettings.pxSheetSize;
    }
}

QSize MainView::getTargetSupportSizePixels()
{
    return targetSupportSizePixels;
}

double MainView::getMinFigureRelativeSize()
{
    return minRelSize;
}

void MainView::exportPDF(QString fileName, SheetSizeType sizeType)
{
    QPdfWriter *pdfWriter = new QPdfWriter(fileName);

    pdfWriter->setCreator(QString("ZeGrapher ") + SOFTWARE_VERSION_STR);
    pdfWriter->setTitle(tr("Exported graph"));

    int targetResolution = int(screenDPI / sizeSettings.scalingFactor);

    pdfWriter->setResolution(targetResolution);   

    QPageLayout layout;
    layout.setUnits(QPageLayout::Millimeter);

    QSizeF sheetSizeMm(sizeSettings.cmSheetSize.width() * 10, sizeSettings.cmSheetSize.height() * 10);
    layout.setPageSize(QPageSize(sheetSizeMm, QPageSize::Millimeter, "", QPageSize::FuzzyOrientationMatch));

    if(sizeType == SheetSizeType::NORMALISED)
        layout.setOrientation(orientation);
    else layout.setOrientation(QPageLayout::Portrait);

    pdfWriter->setPageLayout(layout);

    painter.begin(pdfWriter);

    if(information->getGraphSettings().estheticSettings.backgroundColor != QColor(Qt::white))
    {
        painter.setBrush(QBrush(information->getGraphSettings().estheticSettings.backgroundColor));
        painter.drawRect(painter.viewport());
    }

    figureRectScaled = getFigureRect(painter.viewport());

    painter.translate(figureRectScaled.topLeft());

    paint();

    painter.end();

    delete pdfWriter;
}

void MainView::exportSVG(QString fileName)
{
    QSvgGenerator svgGenerator;
    svgGenerator.setFileName(fileName);

    svgGenerator.setTitle(tr("Exported graph"));
    svgGenerator.setDescription(tr("Created with ZeGrapher ") + SOFTWARE_VERSION_STR);

    double targetResolution = screenDPI / sizeSettings.scalingFactor;

    svgGenerator.setResolution(int(targetResolution));

    QSize sizePx(int(sizeSettings.cmSheetSize.width() * 0.393701 * targetResolution), int(sizeSettings.cmSheetSize.height() * 0.393701 * targetResolution));

    svgGenerator.setSize(sizePx);
    svgGenerator.setViewBox(QRect(QPoint(0, 0), sizePx));

    painter.begin(&svgGenerator);

    if(information->getGraphSettings().estheticSettings.backgroundColor != QColor(Qt::white))
    {
        painter.setBrush(QBrush(information->getGraphSettings().estheticSettings.backgroundColor));
        painter.drawRect(painter.viewport());
    }

    figureRectScaled = getFigureRect(painter.viewport());

    painter.translate(figureRectScaled.topLeft());

    paint();

    painter.end();
}


void MainView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    painter.begin(this);    

    drawSupport();
    drawFigureRect();

    assignMouseRects();

    drawGraph();

    painter.end();
}

void MainView::assignMouseRects()
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

QRect MainView::getDrawableRect(const QRect &refSupportRect)
{   // gives the drawable rect in the given support, in the support's coordinates
    // the drawable rect is the support's rect minus the margins

    QSize supportSize(refSupportRect.size());
    QPoint marginTopLeft;

    if(information->getGraphSizeSettings().sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        marginTopLeft.setX(int(sizeSettings.cmMargins / sizeSettings.cmSheetSize.width() * double(supportSize.width())));
        marginTopLeft.setY(int(sizeSettings.cmMargins / sizeSettings.cmSheetSize.height() * double(supportSize.height())));
    }
    else
    {
        marginTopLeft.setX(int(double(sizeSettings.pxMargins) / double(sizeSettings.pxSheetSize.width()) * double(supportSize.width())));
        marginTopLeft.setY(int(double(sizeSettings.pxMargins) / double(sizeSettings.pxSheetSize.height()) * double(supportSize.height())));
    }

    QRect drawableRect;
    drawableRect.setWidth(supportSize.width() - 2 * marginTopLeft.x());
    drawableRect.setHeight(supportSize.height() - 2 * marginTopLeft.y());
    drawableRect.moveTopLeft(marginTopLeft + refSupportRect.topLeft());

    return drawableRect;
}

QRect MainView::getFigureRect(const QRect &refSupportRect)
{
    QRect rect;
    QPoint graphRectTopLeft;

    QRect drawableRect = getDrawableRect(refSupportRect);

    graphRectTopLeft.setX(int(relFigRect.topLeft().x() * double(drawableRect.width())));
    graphRectTopLeft.setY(int(relFigRect.topLeft().y() * double(drawableRect.height())));

    rect.setWidth(int(relFigRect.width() * drawableRect.width()));
    rect.setHeight(int(relFigRect.height() * drawableRect.height()));
    rect.moveTopLeft(drawableRect.topLeft() + graphRectTopLeft);

    return rect;
}

QRect MainView::supportRectFromViewRect(QRect viewRect)
{
    const auto &sizeSettings = information->getGraphSizeSettings();

    QRect rect;

    if(sizeSettings.sizingType == ZeSizeSettings::FITWINDOW)
    {
        rect = viewRect;
    }
    else
    {
        double ratio, targetRatio;
        ratio = double(viewRect.height()) / double(viewRect.width());

        if(sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
            targetRatio = sizeSettings.cmSheetSize.height() / sizeSettings.cmSheetSize.width();
        else targetRatio = double(sizeSettings.pxSheetSize.height()) / double(sizeSettings.pxSheetSize.width());

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
    }

    return rect;
}

void MainView::drawSupport()
{ // draws the sheet on an untransformed view
    painter.setBrush(QBrush(information->getGraphSettings().estheticSettings.backgroundColor));;

    supportRect = supportRectFromViewRect(painter.viewport());

    painter.drawRect(supportRect);
}

void MainView::drawFigureRect()
{
    figureRect = getFigureRect(supportRect);

    painter.setBrush(Qt::NoBrush);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(1);
    pen.setColor(information->getAxesSettings().x.color);
    painter.setPen(pen);
    painter.drawRect(figureRect);

    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
}

void MainView::setGraphRange(GraphRange range)
{

}

void MainView::scaleView(const QRect &refSheetRect)
{
    double newZoom;

    if(information->getGraphSizeSettings().sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        newZoom = double(refSheetRect.width()) / double(targetSupportSizePixels.width());
    }
    else
    {       
        newZoom = double(refSheetRect.width()) / double(sizeSettings.pxSheetSize.width());
    }

    if(fabs(newZoom - zoomSettings.zoom) > 0.001)
    {
        ZeZoomSettings zoomSettings = information->getGraphZoomSettings();

        zoomSettings.zoomingType = ZeZoomSettings::CUSTOM;
        zoomSettings.zoom = newZoom;

        information->setGraphZoomSettings(zoomSettings);
    }

    zoomSettings.zoom = newZoom;

    double totalScaleFactor = zoomSettings.zoom * sizeSettings.scalingFactor;


    painter.scale(totalScaleFactor, totalScaleFactor);
}

void MainView::drawGraph()
{
    scaleView(supportRect);

    sheetRectScaled = painter.worldTransform().inverted().mapRect(supportRect);

    figureRectScaled = getFigureRect(sheetRectScaled);

    painter.translate(figureRectScaled.topLeft());

    paint();
}

void MainView::onSizeSettingsChange()
{
    sizeSettings = information->getGraphSizeSettings();

    if(sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        relFigRect.setWidth(sizeSettings.cmFigureSize.width() / (sizeSettings.cmSheetSize.width() - 2*sizeSettings.cmMargins));
        relFigRect.setHeight(sizeSettings.cmFigureSize.height() / (sizeSettings.cmSheetSize.height() - 2*sizeSettings.cmMargins));
    }
    else
    {
        relFigRect.setWidth(double(sizeSettings.pxFigureSize.width()) / double(sizeSettings.pxSheetSize.width() - 2*sizeSettings.pxMargins));
        relFigRect.setHeight(double(sizeSettings.pxFigureSize.height()) / (sizeSettings.pxSheetSize.height() - 2*sizeSettings.pxMargins));
    }

    constrainFigureRectRel();
    updateFigureSize();
    updateTargetSupportSizePx();

    update();
}

void MainView::mousePressEvent(QMouseEvent *event)
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

void MainView::mouseMoveEvent(QMouseEvent *event)
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

        QRect drawableRect = getDrawableRect(supportRect);

        QPointF pt = figureRect.topLeft() - drawableRect.topLeft();
        pt.setX(pt.x() / double(drawableRect.width()));
        pt.setY(pt.y() / double(drawableRect.height()));

        relFigRect.setWidth(double(figureRect.width()) / double(drawableRect.width()));
        relFigRect.setHeight(double(figureRect.height()) / double(drawableRect.height()));
        relFigRect.moveTopLeft(pt);

        constrainFigureRectRel();
        updateFigureSize();
        update();
    }

}

void MainView::updateFigureSize()
{
    if(information->getGraphSizeSettings().sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        QSizeF oldCmFigureSize = sizeSettings.cmFigureSize;

        sizeSettings.cmFigureSize.setWidth(relFigRect.width() * (sizeSettings.cmSheetSize.width() - 2*sizeSettings.cmMargins));
        sizeSettings.cmFigureSize.setHeight(relFigRect.height() * (sizeSettings.cmSheetSize.height() - 2*sizeSettings.cmMargins));

        if(oldCmFigureSize != sizeSettings.cmFigureSize)
        {
            information->setGraphSizeSettings(sizeSettings);
        }
    }
    else
    {
        QSizeF oldPxFigureSize = sizeSettings.pxFigureSize;

        sizeSettings.pxFigureSize.setWidth(int(relFigRect.width() * double(sizeSettings.pxSheetSize.width() - 2*sizeSettings.pxMargins)));
        sizeSettings.pxFigureSize.setHeight(int(relFigRect.height() * double(sizeSettings.pxSheetSize.height() - 2*sizeSettings.pxMargins)));

        if(oldPxFigureSize != sizeSettings.pxFigureSize)
        {
            information->setGraphSizeSettings(sizeSettings);
        }
    }
}

void MainView::constrainFigureRectRel()
{    
    if(relFigRect.width() > 1)
        relFigRect.setWidth(1);
    if(relFigRect.height() > 1)
        relFigRect.setHeight(1);

    if(relFigRect.width() < minRelSize)
        relFigRect.setWidth(minRelSize);
    if(relFigRect.height() < minRelSize)
        relFigRect.setHeight(minRelSize);

    if(relFigRect.left() < 0)
        relFigRect.moveLeft(0);
    if(relFigRect.right() > 1)
        relFigRect.moveRight(1);

    if(relFigRect.bottom() > 1)
        relFigRect.moveBottom(1);
    if(relFigRect.top() < 0)
        relFigRect.moveTop(0);

}

void MainView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
    moveType = NOTHING;
}

void MainView::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
}
