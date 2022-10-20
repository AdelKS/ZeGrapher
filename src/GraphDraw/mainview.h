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

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "basegraphdraw.h"
#include <QPdfWriter>
#include <QPageLayout>
#include <QSvgGenerator>

enum MouseActionType {NOTHING, TOPLEFT_CORNER, TOPRIGHT_CORNER,
                      BOTTOMLEFT_CORNER, BOTTOMRIGHT_CORNER, LEFT_SIDE,
                     TOP_SIDE, RIGHT_SIDE, BOTTOM_SIDE, ALL};

enum SheetSizeType {NORMALISED, CUSTOM};

class MainView : public BaseGraphDraw
{
    Q_OBJECT

public:
    explicit MainView();

    void onSizeUnitChange();
    double getMinFigureRelativeSize();

signals:
    void newZoomValue(double value);
    void widgetResized();

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void onSizeSettingsChange();
    void setGraphRange(GraphRange range);
    void updateWidgetSize();

    void exportPDF(QString fileName, SheetSizeType sizeType);
    void exportSVG(QString fileName);
    void onZoomSettingsChange();


protected:
    void paintEvent(QPaintEvent *event);
    void updateSizeValues();


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
    void updateFigureSize();

    QPageLayout::Orientation orientation;
    double minRelSize;
    QSize currentSize;
    // margin to the sheet where the graph can be, this value is used for the smaller edge of the sheet
    // the other margin is scaled accordingly
    double screenDPI;
    QRect figureRect, supportRect, sheetRectScaled;

    ZeSizeSettings sizeSettings;
    ZeZoomSettings zoomSettings;

    QRectF relFigRect;
    QRect topLeft, topRight, top, left, right, bottom, bottomLeft, bottomRight;

    QPoint lastMousePos ;
    MouseActionType moveType;
    QString fileName;

};

#endif // MAINVIEW_H
