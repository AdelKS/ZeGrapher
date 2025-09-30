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

#include "GraphDraw/interactivegraph.h"
#include "MathObjects/mathworld.h"
#include "information.h"

#include <QQuickWindow>

InteractiveGraph::InteractiveGraph(QQuickItem *parent) : Graph(parent)
{
  orientation = QPageLayout::Landscape;
  moveType = NOTHING;
  sizeSettings.scalingFactor = 1;
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);

  minRelSize = RELATIVE_MIN_SIZE;

  screenDPI = qGuiApp->primaryScreen()->physicalDotsPerInch();

  relFigRect.setHeight(1);
  relFigRect.setWidth(1);
  relFigRect.moveTopLeft(QPointF(0, 0));

  onSizeSettingsChange();

  connect(&information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(onSizeSettingsChange()));
  connect(&information, SIGNAL(graphZoomSettingsChanged()), this, SLOT(onZoomSettingsChange()));
  connect(&information, SIGNAL(axesSettingsChanged()), this, SLOT(update()));
  connect(&information, SIGNAL(gridSettingsChanged()), this, SLOT(update()));
  connect(&information, SIGNAL(dataUpdated()), this, SLOT(update()));
  connect(&zg::mathWorld, &zg::MathWorld::updated, this, [this]{ update(); });
}

void InteractiveGraph::updateWidgetSize()
{
  QSizeF newSize;
  if (sizeSettings.sheetFillsWindow or zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
    newSize = parentItem()->size();
  else
    newSize = QSizeF(sizeSettings.pxSheetSize) * zoomSettings.zoom;

  setSize(newSize);
  updateFigureSize();

  emit widgetResized();
}

double InteractiveGraph::getMinFigureRelativeSize()
{
  return minRelSize;
}

void InteractiveGraph::exportPDF(QString fileName, SheetSizeType sizeType)
{
  QPdfWriter *pdfWriter = new QPdfWriter(fileName);

  pdfWriter->setCreator(QString("ZeGrapher ") + SOFTWARE_VERSION_STR);
  pdfWriter->setTitle(tr("Exported graph"));

  int targetResolution = int(screenDPI / sizeSettings.scalingFactor);

  pdfWriter->setResolution(targetResolution);

  QPageLayout layout;
  layout.setUnits(QPageLayout::Millimeter);

  QSizeF sheetSizeMm(sizeSettings.cmSheetSize.width() * 10, sizeSettings.cmSheetSize.height() * 10);
  layout.setPageSize(
    QPageSize(sheetSizeMm, QPageSize::Millimeter, "", QPageSize::FuzzyOrientationMatch));

  if (sizeType == SheetSizeType::NORMALISED)
    layout.setOrientation(orientation);
  else
    layout.setOrientation(QPageLayout::Portrait);

  pdfWriter->setPageLayout(layout);

  painter->begin(pdfWriter);

  if (information.getGraphSettings().backgroundColor != QColor(Qt::white))
  {
    painter->setBrush(QBrush(information.getGraphSettings().backgroundColor));
    painter->drawRect(painter->viewport());
  }

  figureRectScaled = getFigureRect(painter->viewport());

  painter->translate(figureRectScaled.topLeft());

  drawAll();

  painter->end();

  delete pdfWriter;
}

void InteractiveGraph::exportSVG(QString fileName)
{
  QSvgGenerator svgGenerator;
  svgGenerator.setFileName(fileName);

  svgGenerator.setTitle(tr("Exported graph"));
  svgGenerator.setDescription(tr("Created with ZeGrapher ") + SOFTWARE_VERSION_STR);

  double targetResolution = screenDPI / sizeSettings.scalingFactor;

  svgGenerator.setResolution(int(targetResolution));

  QSize sizePx(int(sizeSettings.cmSheetSize.width() * 0.393701 * targetResolution),
               int(sizeSettings.cmSheetSize.height() * 0.393701 * targetResolution));

  svgGenerator.setSize(sizePx);
  svgGenerator.setViewBox(QRect(QPoint(0, 0), sizePx));

  painter->begin(&svgGenerator);

  if (information.getGraphSettings().backgroundColor != QColor(Qt::white))
  {
    painter->setBrush(QBrush(information.getGraphSettings().backgroundColor));
    painter->drawRect(painter->viewport());
  }

  figureRectScaled = getFigureRect(painter->viewport());

  painter->translate(figureRectScaled.topLeft());

  drawAll();

  painter->end();
}

void InteractiveGraph::updateSizeValues()
{
  zoomSettings = information.getGraphZoomSettings();
  sizeSettings = information.getGraphSizeSettings();

  if (sizeSettings.figureFillsSheet)
  {
    sizeSettings.cmMargins = 0;
    sizeSettings.pxMargins = 0;
  }

  if (sizeSettings.sheetFillsWindow)
  {
    sizeSettings.pxSheetSize = size().toSize();
    sizeSettings.cmSheetSize = sizeSettings.pxSheetSize / screenDPI * CM_PER_INCH;
  }

  if (sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
  {
    sizeSettings.pxSheetSize = (sizeSettings.cmSheetSize * screenDPI / CM_PER_INCH).toSize();
  }

  disconnect(&information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(onSizeSettingsChange()));
  information.setGraphSizeSettings(sizeSettings);
  connect(&information, SIGNAL(graphSizeSettingsChanged()), this, SLOT(onSizeSettingsChange()));
}

void InteractiveGraph::paint(QPainter *p)
{
  updateSizeValues();

  if (currentSize != size())
  {
    currentSize = size();
    updateWidgetSize();
    onSizeSettingsChange();
  }

  painter = p;

  drawSupport();

  if (not sizeSettings.figureFillsSheet)
    drawFigureRect();

  assignMouseRects();

  drawGraph();

  painter = nullptr;
}

void InteractiveGraph::assignMouseRects()
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

QRect InteractiveGraph::getDrawableRect(const QRect &refSupportRect)
{ // gives the drawable rect in the given support, in the support's coordinates
  // the drawable rect is the support's rect minus the margins

  int pxMargins = 0;

  if (information.getGraphSizeSettings().sizeUnit == ZeSizeSettings::CENTIMETER)
  {
    pxMargins = int(sizeSettings.cmMargins / CM_PER_INCH * screenDPI);
  }
  else
  {
    pxMargins = sizeSettings.pxMargins;
  }

  QRect drawableRect;
  drawableRect.setWidth(refSupportRect.width() - 2 * pxMargins);
  drawableRect.setHeight(refSupportRect.height() - 2 * pxMargins);
  drawableRect.moveTopLeft(QPoint(pxMargins, pxMargins));

  return drawableRect;
}

QRect InteractiveGraph::getFigureRect(const QRect &refSupportRect)
{
  int margin = sizeSettings.pxMargins;
  QMargins margins(margin, margin, margin, margin);

  QRect refSupportRectMarginless = refSupportRect.marginsRemoved(margins);
  QRect figRect;
  figRect.setWidth(lround(relFigRect.width() * double(refSupportRectMarginless.width())));
  figRect.setHeight(lround(relFigRect.height() * double(refSupportRectMarginless.height())));

  QPoint topLeft;
  topLeft.setX(lround(relFigRect.topLeft().x() * double(refSupportRectMarginless.width()))
               + refSupportRectMarginless.topLeft().x());
  topLeft.setY(lround(relFigRect.topLeft().y() * double(refSupportRectMarginless.height()))
               + refSupportRectMarginless.topLeft().y());

  figRect.moveTopLeft(topLeft);

  // qDebug() << "Figure reconstruction call";
  // qDebug() << "Figure is " << figRect;
  // qDebug() << "marginless support is " << refSupportRectMarginless;
  // qDebug() << "relative figure rect is " << relFigRect;

  return figRect;
}

QRect InteractiveGraph::supportRectFromViewRect(QRect viewRect)
{
  QRect rect;

  if (sizeSettings.sheetFillsWindow)
  {
    rect = viewRect;
  }
  else if (zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
  {
    double ratio, targetRatio;
    ratio = double(viewRect.height()) / double(viewRect.width());

    if (sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
      targetRatio = sizeSettings.cmSheetSize.height() / sizeSettings.cmSheetSize.width();
    else
      targetRatio = double(sizeSettings.pxSheetSize.height())
                    / double(sizeSettings.pxSheetSize.width());

    if (ratio <= targetRatio)
    {
      rect.setHeight(viewRect.height());
      rect.setWidth(int(double(viewRect.height()) / targetRatio));

      rect.moveTopLeft(QPoint((viewRect.width() - rect.width()) / 2, 0));
    }
    else
    {
      rect.setHeight(int(double(viewRect.width()) * targetRatio));
      rect.setWidth(viewRect.width());

      rect.moveTopLeft(QPoint(0, (viewRect.height() - rect.height()) / 2));
    }
  }
  else
  {
    rect.setSize((QSizeF(sizeSettings.pxSheetSize) * zoomSettings.zoom).toSize());
    rect.translate(viewRect.center() - rect.center());
  }

  return rect;
}

void InteractiveGraph::drawSupport()
{ // draws the sheet on an untransformed view
  painter->setBrush(QBrush(information.getGraphSettings().backgroundColor));

  supportRect = supportRectFromViewRect(painter->viewport());

  painter->drawRect(supportRect);
}

void InteractiveGraph::drawFigureRect()
{
  figureRect = getFigureRect(supportRect);

  painter->setBrush(Qt::NoBrush);
  pen.setStyle(Qt::DashLine);
  pen.setWidth(1);
  pen.setColor(information.getAxesSettings().color);
  painter->setPen(pen);
  painter->drawRect(figureRect);

  pen.setStyle(Qt::SolidLine);
  painter->setPen(pen);
}

void InteractiveGraph::scaleView(const QRect &refSheetRect)
{
  if (zoomSettings.zoomingType == ZeZoomSettings::FITSHEET)
  {
    double newZoom = double(refSheetRect.width()) / double(sizeSettings.pxSheetSize.width());

    if (fabs(newZoom - zoomSettings.zoom) > 0.001)
    {
      zoomSettings.zoom = newZoom;

      information.setGraphZoomSettings(zoomSettings);
    }
  }

  double totalScaleFactor = zoomSettings.zoom * sizeSettings.scalingFactor;

  qDebug() << "Graph size: " << size();
  qDebug() << "Painter viewport (before scaling): " << painter->viewport();
  qDebug() << "Painter window (before scaling): " << painter->window();

  pixelRatio = window()->effectiveDevicePixelRatio();

  pixelRatioTransform.reset();
  pixelRatioTransform.scale(pixelRatio, pixelRatio);

  inversePixelRatioTransform.reset();
  inversePixelRatioTransform.scale(1./pixelRatio, 1./pixelRatio);

  qDebug() << "Painter scaled back window (before scaling): "
           << inversePixelRatioTransform.mapRect(painter->window());

  qDebug() << "Pixel ratio: " << pixelRatio;

  painter->scale(totalScaleFactor, totalScaleFactor);

  worldTransform = painter->worldTransform();
  inverseWorldTransform = painter->worldTransform().inverted();

  qDebug() << "Painter viewport (after scaling): " << painter->viewport();
  qDebug() << "Painter window (after scaling): " << painter->window();
}

void InteractiveGraph::drawGraph()
{
  scaleView(supportRect);

  sheetRectScaled = inverseWorldTransform.mapRect(supportRect);

  figureRectScaled = getFigureRect(sheetRectScaled);

  painter->translate(figureRectScaled.topLeft());

  drawAll();
}

void InteractiveGraph::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton and boundingRect().contains(event->pos()))
    moveType = MOVE_VIEW;
  else if (not sizeSettings.figureFillsSheet)
  {
    if (topLeft.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOPLEFT_CORNER;
    else if (topRight.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOPRIGHT_CORNER;
    else if (top.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOP_SIDE;
    else if (bottomLeft.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOMLEFT_CORNER;
    else if (bottomRight.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOMRIGHT_CORNER;
    else if (bottom.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOM_SIDE;
    else if (left.contains(event->pos()))
      moveType = RESIZE_GRAPH_LEFT_SIDE;
    else if (right.contains(event->pos()))
      moveType = RESIZE_GRAPH_RIGHT_SIDE;
    else if (figureRect.contains(event->pos()))
      moveType = MOVE_GRAPH;
    else
      moveType = NOTHING;
  }

  if (moveType != NOTHING)
  {
    lastMousePos = inversePixelRatioTransform.map(event->pos());
    event->accept();
  }
}

void InteractiveGraph::mouseMoveEvent(QMouseEvent *event)
{
  if (moveType == NOTHING and not sizeSettings.figureFillsSheet)
  {
    if (topLeft.contains(event->pos()) || bottomRight.contains(event->pos()))
      setCursor(Qt::SizeFDiagCursor);
    else if (topRight.contains(event->pos()) || bottomLeft.contains(event->pos()))
      setCursor(Qt::SizeBDiagCursor);
    else if (top.contains(event->pos()) || bottom.contains(event->pos()))
      setCursor(Qt::SizeVerCursor);
    else if (left.contains(event->pos()) || right.contains(event->pos()))
      setCursor(Qt::SizeHorCursor);
    else if (figureRect.contains(event->pos()))
      setCursor(Qt::SizeAllCursor);
    else
      setCursor(Qt::ArrowCursor);

    lastMousePos = inverseWorldTransform.map(event->pos());
  }
  else
  {
    qDebug() << "################################";
    qDebug() << random();
    qDebug() << "Mouse move event";
    qDebug() << "figure Rect before change: " << figureRect;

    auto update_relfigrect = [this]{
      qDebug() << "figure Rect after change: " << figureRect;

      int marginVal = sizeSettings.pxMargins;
      QMargins margins(marginVal, marginVal, marginVal, marginVal);
      QRect marginlessSupport = supportRect.marginsRemoved(margins);

      QPointF topLeft = figureRect.topLeft() - marginlessSupport.topLeft();
      topLeft.setX(topLeft.x() / double(marginlessSupport.width()));
      topLeft.setY(topLeft.y() / double(marginlessSupport.height()));

      relFigRect.setWidth(double(figureRect.width()) / double(marginlessSupport.width()));
      relFigRect.setHeight(double(figureRect.height()) / double(marginlessSupport.height()));
      relFigRect.moveTopLeft(topLeft);

      qDebug() << "marginless support is " << marginlessSupport;
      qDebug() << "relative figure rect is " << relFigRect;

      constrainFigureRectRel();
      updateFigureSize();
      update();
    };

    QPoint pos = inversePixelRatioTransform.map(event->pos());

    qDebug() << "Graph size: " << size();
    qDebug() << "Mouse position: " << pos;

    QPoint dr = pos - lastMousePos;
    lastMousePos = pos;

    switch (moveType)
    {
    case MOVE_VIEW:
      // the view goes in the opposite direction
      viewMapper.translateView({
        .x = zg::pixel_unit{double(-dr.x())},
        .y = zg::pixel_unit{double(-dr.y())},
      });
      information.setGraphRange(viewMapper.getRange<zg::real>().toGraphRange());
      break;
    case RESIZE_GRAPH_TOPLEFT_CORNER:
      figureRect.setTopLeft(figureRect.topLeft() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_TOPRIGHT_CORNER:
      figureRect.setTopRight(figureRect.topRight() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOMLEFT_CORNER:
      figureRect.setBottomLeft(figureRect.bottomLeft() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOMRIGHT_CORNER:
      figureRect.setBottomRight(figureRect.bottomRight() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_LEFT_SIDE:
      figureRect.setLeft(figureRect.left() + dr.x());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_TOP_SIDE:
      figureRect.setTop(figureRect.top() + dr.y());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_RIGHT_SIDE:
      figureRect.setRight(figureRect.right() + dr.x());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOM_SIDE:
      figureRect.setBottom(figureRect.bottom() + dr.y());
      update_relfigrect();
      break;
    case MOVE_GRAPH:
      figureRect.translate(dr);
      update_relfigrect();
      break;
    case NOTHING:
      break;
    }
  }
}

void InteractiveGraph::onSizeSettingsChange()
{
  // Add function here that calcualtes the needed widget size

  // qDebug() << "OnSizeSettingsChange";

  sizeSettings = information.getGraphSizeSettings();

  if (sizeSettings.figureFillsSheet)
  {
    relFigRect.setTopLeft(QPointF(0, 0));
    relFigRect.setWidth(1);
    relFigRect.setHeight(1);
  }
  else
  {
    if (sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
    {
      relFigRect.setWidth(sizeSettings.cmFigureSize.width() / (sizeSettings.cmSheetSize.width()));
      relFigRect.setHeight(sizeSettings.cmFigureSize.height() / (sizeSettings.cmSheetSize.height()));
    }
    else
    {
      relFigRect.setWidth(double(sizeSettings.pxFigureSize.width())
                          / double(sizeSettings.pxSheetSize.width()));
      relFigRect.setHeight(double(sizeSettings.pxFigureSize.height())
                           / (sizeSettings.pxSheetSize.height()));
    }

    constrainFigureRectRel();
  }

  updateFigureSize();
  update();
}

void InteractiveGraph::onZoomSettingsChange()
{
  updateWidgetSize();
  update();
}

void InteractiveGraph::updateFigureSize()
{
  sizeSettings.cmFigureSize.setWidth(relFigRect.width() * sizeSettings.cmSheetSize.width());
  sizeSettings.cmFigureSize.setHeight(relFigRect.height() * sizeSettings.cmSheetSize.height());

  sizeSettings.pxFigureSize.setWidth(
    int(relFigRect.width() * double(sizeSettings.pxSheetSize.width())));
  sizeSettings.pxFigureSize.setHeight(
    int(relFigRect.height() * double(sizeSettings.pxSheetSize.height())));
}

void InteractiveGraph::constrainFigureRectRel()
{
  if (relFigRect.width() > 1)
    relFigRect.setWidth(1);

  if (relFigRect.height() > 1)
    relFigRect.setHeight(1);
  //------------------------------

  if (relFigRect.width() < minRelSize)
    relFigRect.setWidth(minRelSize);

  if (relFigRect.height() < minRelSize)
    relFigRect.setHeight(minRelSize);
  //------------------------------

  if (relFigRect.left() < 0)
    relFigRect.moveLeft(0);

  if (relFigRect.right() > 1)
    relFigRect.moveRight(1);
  //------------------------------

  if (relFigRect.bottom() > 1)
    relFigRect.moveBottom(1);

  if (relFigRect.top() < 0)
    relFigRect.moveTop(0);
}

void InteractiveGraph::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  setCursor(Qt::ArrowCursor);
  moveType = NOTHING;
}

void InteractiveGraph::wheelEvent(QWheelEvent *event)
{
  Q_UNUSED(event);
}
