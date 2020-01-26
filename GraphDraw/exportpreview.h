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

#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include "basegraphdraw.h"
#include <QPdfWriter>
#include <QPageLayout>
#include <QSvgGenerator>

#define RELATIVE_MIN_SIZE 0.25

enum MouseActionType {NOTHING, TOPLEFT_CORNER, TOPRIGHT_CORNER,
                      BOTTOMLEFT_CORNER, BOTTOMRIGHT_CORNER, LEFT_SIDE,
                     TOP_SIDE, RIGHT_SIDE, BOTTOM_SIDE, ALL};

enum ExportType {VECTORIAL, IMAGE};
enum SheetSizeType {NORMALISED, CUSTOM};



class ExportPreview : public BaseGraphDraw
{
    Q_OBJECT

public:
    explicit ExportPreview(Information *info);

    void setExportType(ExportType type);
    double getMinFigureRelativeSize();
    QSize getTargetSupportSizePixels();

signals:
    void newFigureSizeCm(QSizeF sizeCm);
    void newFigureSizePx(QSize sizePx);

    void newZoomValue(double value);

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void setSheetFigureSizeCm(QSizeF sizeCm);
    void setImageFigureSizePx(QSize sizePx);

    void setSheetSizeCm(QSizeF sizeCm);
    void setImageSizePx(QSize sizePx);

    void setSheetMarginCm(double sheetMarginCm);
    void setImageMarginPx(int imageMarginPx);

    void setGraphRange(GraphRange range);

    void setScale(double scalingFactor);

    void exportPDF(QString fileName, SheetSizeType sizeType);
    void exportSVG(QString fileName);

    void setSheetOrientation(QPageLayout::Orientation type);


protected:
    void paintEvent(QPaintEvent *event);

    void initialise();

    void drawSupport();
    void drawGraph();
    QRect supportRectFromViewRect(QRect viewRect);
    void drawFigureRect();
    void assignMouseRects();
    void printCurves();
    void constrainFigureRectRel();
    QRect getFigureRect(const QRect &refSupportRect);
    QRect getDrawableRect(const QRect &refSupportRect);
    void scaleView(const QRect &refSheetRect);
    void setMaximalCanvas();
    void updateTargetSupportSizePx();
    void updateFigureSize();

    QPageLayout::Orientation orientation;
    double minRelSize;
    double sheetMarginCm;
    int imageMarginPx;
    // margin to the sheet where the graph can be, this value is used for the smaller edge of the sheet
    // the other margin is scaled accordingly
    double userScalingFactor, screenDPI, currentZoom;
    QSize targetSupportSizePixels;
    QRect figureRect, supportRect, sheetRectScaled;
    QSizeF figureSizeCm, sheetSizeCm;
    QSize figureSizePx, imageSizePx;

    QRectF sheetFigureRectRelative, imageFigureRectRelative;
    QRect topLeft, topRight, top, left, right, bottom, bottomLeft, bottomRight;

    QPoint lastMousePos ;
    MouseActionType moveType;
    QString fileName;

    ExportType exportType;

};

#endif // PRINTPREVIEW_H
