/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

#include "graph.h"
#include <QPageLayout>
#include <QPdfWriter>
#include <QSvgGenerator>

enum MouseActionType
{
  NOTHING,
  MOVE_VIEW,
  RESIZE_GRAPH_TOPLEFT_CORNER,
  RESIZE_GRAPH_TOPRIGHT_CORNER,
  RESIZE_GRAPH_BOTTOMLEFT_CORNER,
  RESIZE_GRAPH_BOTTOMRIGHT_CORNER,
  RESIZE_GRAPH_LEFT_SIDE,
  RESIZE_GRAPH_TOP_SIDE,
  RESIZE_GRAPH_RIGHT_SIDE,
  RESIZE_GRAPH_BOTTOM_SIDE,
  MOVE_GRAPH
};

enum SheetSizeType
{
  NORMALISED,
  CUSTOM
};

class InteractiveGraph : public Graph
{
  Q_OBJECT
  QML_ELEMENT

public:
  explicit InteractiveGraph(QQuickItem *parent = nullptr);

  void onSizeUnitChange();
  double getMinFigureRelativeSize();

  virtual void paint(QPainter *p) override;

signals:
  void newZoomValue(double value);
  void widgetResized();

public slots:
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void wheelEvent(QWheelEvent *event) override;

  Q_INVOKABLE void onSizeSettingsChange();

  void exportPDF(QString fileName, SheetSizeType sizeType);
  void exportSVG(QString fileName);
  void onZoomSettingsChange();

  Q_INVOKABLE void updateFigureSize();

protected:

  void drawSupport();
  void drawGraph();
  QRectF supportRectFromViewSize(QSizeF);
  void drawFigureRect();
  void assignMouseRects();
  void printCurves();
  void constrainFigureRectRel();
  QRect getFigureRect(const QRect &refSupportRect);
  QRect getDrawableRect(const QRect &refSupportRect);
  void scaleView(const QRect &refSheetRect);
  void setMaximalCanvas();

  QPageLayout::Orientation orientation;
  double minRelSize;
  QSizeF currentSize;
  // margin to the sheet where the graph can be, this value is used for the smaller edge of the sheet
  // the other margin is scaled accordingly
  double pixelRatio;
  QRect figureRect, supportRect, sheetRectScaled;

  ZeSizeSettings sizeSettings;
  ZeZoomSettings zoomSettings;

  QTransform worldTransform, inverseWorldTransform, inversePixelRatioTransform;

  QRectF relFigRect;
  QRect topLeft, topRight, top, left, right, bottom, bottomLeft, bottomRight;

  QPoint lastMousePos;
  MouseActionType moveType;
  QString fileName;
};

#endif // MAINVIEW_H
