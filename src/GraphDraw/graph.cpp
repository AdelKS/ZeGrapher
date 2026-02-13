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

#include "GraphDraw/graph.h"
#include "globalvars.h"

#include <QQuickWindow>
#include <QSvgGenerator>

Graph::Graph(QQuickItem *parent)
  : QQuickPaintedItem(parent), MathObjectDraw(), gridCalculator(this),
    fontMetrics(information.getGraphSettings().graphFont)
{
  leftMargin = 30;
  rightMargin = 30;
  topMargin = 20;
  bottomMargin = 30;
  pen.setCapStyle(Qt::RoundCap);

  legendFontSize = 12;
  legendState = false;
  additionalMargin = 0;
  bold = italic = underline = false;
  numPrec = NUM_PREC;

  viewMapper.setGraphRange(information.getGraphRange()->getLatestValidSnapshot());

  minRelSize = RELATIVE_MIN_SIZE;

  relFigRect.setHeight(1);
  relFigRect.setWidth(1);
  relFigRect.moveTopLeft(QPointF(0, 0));

  connect(&information, SIGNAL(estheticSettingsChanged()), this, SLOT(update()));
  connect(&information, SIGNAL(updateOccured()), this, SLOT(update()));
  connect(&information, SIGNAL(viewSettingsChanged()), this, SLOT(updateSettingsVals()));
  connect(&information, SIGNAL(styleUpdated()), this, SLOT(update()));

  connect(&information, &Information::graphSizeSettingsChanged, this, &Graph::onSizeSettingsChange);
  connect(&information, &Information::graphZoomSettingsChanged, this, &Graph::onZoomSettingsChange);
  connect(&information, &Information::axesSettingsChanged, this, [this]{ update(); });
  connect(&information, &Information::gridSettingsChanged, this, [this]{ update(); });
  connect(&information, &Information::dataUpdated, this, [this]{ update(); });
  connect(&zg::mathWorld, &zg::MathWorld::updated, this, [this]{ update(); });
}

void Graph::updateSettingsVals()
{
  sampler.setPixelStep(information.getGraphSettings().distanceBetweenPoints);
}

void Graph::setNumPrec(int prec)
{
  numPrec = prec;
  update();
}

void Graph::setBold(bool state)
{
  bold = state;
  update();
}

void Graph::setUnderline(bool state)
{
  underline = state;
  update();
}

void Graph::setItalic(bool state)
{
  italic = state;
  update();
}

void Graph::setlegendFontSize(int size)
{
  if (legendState)
  {
    leftMargin -= legendFontSize;
    leftMargin += size;

    bottomMargin -= legendFontSize;
    bottomMargin += size;

    additionalMargin = size + 10;
  }

  legendFontSize = size;

  update();
}

void Graph::setLegendState(bool show)
{
  if (show && !legendState)
  {
    leftMargin += legendFontSize + 10;
    bottomMargin += legendFontSize + 10;

    additionalMargin = legendFontSize + 10;
  }
  else if (!show && legendState)
  {
    leftMargin -= legendFontSize + 10;
    bottomMargin -= legendFontSize + 10;

    additionalMargin = 0;
  }

  legendState = show;

  update();
}

void Graph::setXaxisLegend(QString legend)
{
  xLegend = legend;
  update();
}

void Graph::setYaxisLegend(QString legend)
{
  yLegend = legend;
  update();
}

void Graph::updateGraphRect()
{
  graphRectScaled.setWidth(figureRectScaled.width() - leftMargin - rightMargin);
  graphRectScaled.setHeight(figureRectScaled.height() - topMargin - bottomMargin);
  graphRectScaled.moveTopLeft(QPoint(0, 0)); // because painter is translated to its top-left corner
  qDebug() << "Graph rect scaled " << graphRectScaled;
  viewMapper.setGraphRect(graphRectScaled);
}

void Graph::calculateTicksAndMargins()
{
  updateGraphRect();

  xAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.x, fontMetrics);
  yAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.y, fontMetrics);

  if(xAxisTicks.offset.sumOffset != 0)
  {
    int margin = fontMetrics.boundingRect(xAxisTicks.offset.sumOffsetStr()).width() + 5;
    if(rightMargin < margin)
      rightMargin = 10 + margin;
  }

  if(xAxisTicks.offset.basePowerOffset != 0)
  {
    int margin = fontMetrics.boundingRect(xAxisTicks.offset.basePowerOffsetStr()).width() + 5;
    rightMargin = 10 + margin;
  }

  if(xAxisTicks.offset.basePowerOffset == 0 && xAxisTicks.offset.sumOffset == 0)
    rightMargin = 10;

  leftMargin = yAxisTicks.maxPxWidth + additionalMargin + 10;

  int offset_margin = 0;
  if (yAxisTicks.offset.basePowerOffset != 0)
  {
    QString power_offset = yAxisTicks.offset.basePowerOffsetStr();

    offset_margin = fontMetrics.boundingRect(power_offset).height();
  }
  if (yAxisTicks.offset.sumOffset != 0)
  {
    QString sum_offset = yAxisTicks.offset.sumOffsetStr();

    int new_offsetmargin = fontMetrics.boundingRect(sum_offset).height();
    if (new_offsetmargin > offset_margin)
      offset_margin = new_offsetmargin;
  }

  topMargin = std::max(20, 5 + offset_margin);

  updateGraphRect();
}

void Graph::paint(QPainter *p)
{
  sizeSettings = information.getGraphSizeSettings();

  if (currentSize != size())
  {
    currentSize = size();
    onSizeSettingsChange();
  }

  painter = p;

  scaledSize = size();

  scaleView();
  drawSupport();

  if (not sizeSettings.figureFillsSheet)
    drawFigureRect();

  drawGraph();

  painter = nullptr;
}

void Graph::drawAll()
{
  painter->setFont(information.getGraphSettings().graphFont);
  fontMetrics = painter->fontMetrics();
  viewMapper.setGraphRange(information.getGraphRange()->getLatestValidSnapshot());

  calculateTicksAndMargins();
  calculateTicksAndMargins();
  calculateTicksAndMargins();

  updateCenterPosAndScaling();

  painter->translate(leftMargin, topMargin);

  drawGraphRect();

  if(information.getAxesSettings().x.axisType == ZeViewType::LINEAR)
  {
    writeAxisOffsetX();
    drawLinAxisGridTicks<ZeAxisName::X>();
  }

  if(information.getAxesSettings().y.axisType == ZeViewType::LINEAR)
  {
    writeAxisOffsetY();
    drawLinAxisGridTicks<ZeAxisName::Y>();
  }

  if(legendState)
      writeLegends();

  painter->setClipRect(graphRectScaled);

  sampler.update();
  drawObjects();

  painter->translate(QPointF(centre.x, centre.y));
}

void Graph::scaleView()
{
  pixelRatio = window()->effectiveDevicePixelRatio();

  qDebug() << "Scaling: " << sizeSettings.scalingFactor;
  qDebug() << "Zoom: " << zoomSettings.zoom;

  totalScaleFactor = sizeSettings.scalingFactor * zoomSettings.zoom;

  // the qpainter is still drawing to the actual buffer-size that's given by painter-viewport
  // this is just telling it to draw everything bigger and translate coordinates accordingly
  painter->scale(totalScaleFactor, totalScaleFactor);

  // this is the new "view size": i.e. the whole widget spans that size, drawing anything above goes out
  scaledSize /= totalScaleFactor;

  qDebug() << "Pixel ratio: " << pixelRatio;

  qDebug() << "Available widget size: " << information.getAvailableSheetSizePx();
  qDebug() << "Graph size: " << size();
  qDebug() << "Painter viewport: " << painter->viewport();
  qDebug() << "Painter window: " << painter->window();
}

void Graph::drawGraph()
{
  qDebug() << "Support rect " << supportRect;

  sheetRectScaled = supportRect;

  qDebug() << "Sheet rect scaled " << sheetRectScaled;

  figureRectScaled = getFigureRect(sheetRectScaled);

  qDebug() << "Figure rect scaled " << figureRectScaled;

  painter->translate(figureRectScaled.topLeft());

  drawAll();
}


void Graph::writeLegends()
{
  QFont font = information.getGraphSettings().graphFont;
  font.setPixelSize(legendFontSize);
  font.setItalic(italic);
  font.setBold(bold);
  font.setUnderline(underline);

  painter->setFont(font);

  if (!xLegend.isEmpty())
  {
    int xLegendWidth = painter->fontMetrics().boundingRect(xLegend).width();

    QPoint startDrawPoint;
    startDrawPoint.setX((graphRectScaled.width() - xLegendWidth) / 2);
    startDrawPoint.setY(graphRectScaled.height() + bottomMargin - 10);

    painter->drawText(startDrawPoint, xLegend);
  }
  if (!yLegend.isEmpty())
  {
    painter->rotate(-90);
    int yLegendWidth = painter->fontMetrics().boundingRect(yLegend).width();
    int yLegendHeight = legendFontSize + 6;

    QPoint startDrawPoint;
    startDrawPoint.setX(-(graphRectScaled.height() - (graphRectScaled.height() - yLegendWidth) / 2));
    startDrawPoint.setY(-leftMargin + yLegendHeight);

    painter->drawText(startDrawPoint, yLegend);

    painter->rotate(90);
  }
}

void Graph::writeAxisOffsetX()
{
  pen.setColor(information.getAxesSettings().color);
  pen.setWidthF(information.getAxesSettings().lineWidth);
  painter->setPen(pen);
  painter->setRenderHint(QPainter::Antialiasing, true);

  const auto& offset = xAxisTicks.offset;

  if(offset.sumOffset != 0)
  {
    QString sum_offset = offset.sumOffsetStr();

    painter->drawText(QPointF(graphRectScaled.width() + 5,
                             graphRectScaled.height()
                               - fontMetrics.boundingRect(sum_offset).height() - 10),
                     sum_offset);

    int margin = fontMetrics.boundingRect(sum_offset).width() + 5;
    Q_ASSERT(rightMargin >= margin); // Should be handled with calculateTicksAndMargins
  }

  if(offset.basePowerOffset != 0)
  {
    QString power_offset = offset.basePowerOffsetStr();

    painter->drawText(QPointF(graphRectScaled.width() + 5, graphRectScaled.height()), power_offset);

    int margin = fontMetrics.boundingRect(power_offset).width() + 5;
    Q_ASSERT(rightMargin >= margin); // Should be handled with calculateTicksAndMargins
  }
}

void Graph::writeAxisOffsetY()
{
  int powerOffset_size = 0;
  int offset_margin = 0;
  if (yAxisTicks.offset.basePowerOffset != 0)
  {
    QString power_offset = yAxisTicks.offset.basePowerOffsetStr();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawText(QPointF(0, -4), power_offset);

    powerOffset_size = fontMetrics.boundingRect(power_offset).width() + 5;
  }
  if (yAxisTicks.offset.sumOffset != 0)
  {
    QString sum_offset = yAxisTicks.offset.sumOffsetStr();

    int new_offsetmargin = fontMetrics.boundingRect(sum_offset).height();
    if (new_offsetmargin > offset_margin)
      offset_margin = new_offsetmargin;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawText(QPointF(powerOffset_size + 5, -4), sum_offset);
  }
}

void Graph::drawGraphRect()
{
  const auto &axesSettings = information.getAxesSettings();

  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->setBrush(QBrush(Qt::NoBrush));

  pen.setWidth(axesSettings.lineWidth);
  pen.setColor(axesSettings.color);
  pen.setStyle(Qt::SolidLine);
  painter->setPen(pen);

  painter->drawRect(graphRectScaled);
}

void Graph::updateCenterPosAndScaling()
{
  // TODO: update this method not working here

  pxPerUnit.y = double(graphRectScaled.height())
                / fabs(std::as_const(viewMapper).y.getRange<zg::plane::view>().amplitude().v);
  pxPerUnit.x = double(graphRectScaled.width())
                / fabs(std::as_const(viewMapper).x.getRange<zg::plane::view>().amplitude().v);

  if (information.getAxesSettings().orthonormal)
  {
    // TODO
  }

  centre.x = -viewMapper.x.getMin<zg::plane::view>().v * pxPerUnit.x;
  centre.y = -viewMapper.y.getMin<zg::plane::view>().v * pxPerUnit.y;
}

QImage *Graph::drawImage()
{
  QImage *image = new QImage(size().toSize(), QImage::Format_RGB32);
  image->fill(information.getGraphSettings().backgroundColor.rgb());

  QPainter p(image);
  painter = &p;

  drawAll();

  painter = nullptr;

  //*image = image->convertToFormat(QImage::Format_Indexed8, Qt::DiffuseDither);
  return image;
}

QRect Graph::getFigureRect(const QRect &refSupportRect)
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

  qDebug() << "Figure reconstruction call";
  qDebug() << "Figure is " << figRect;
  qDebug() << "marginless support is " << refSupportRectMarginless;
  qDebug() << "relative figure rect is " << relFigRect;

  return figRect;
}

void Graph::drawSupport()
{ // draws the sheet on an untransformed view
  painter->setBrush(QBrush(information.getGraphSettings().backgroundColor));

  computeSupportRect();

  painter->drawRect(supportRect);
}

void Graph::drawFigureRect()
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

void Graph::computeSupportRect()
{
  QRectF rect;

  if (sizeSettings.sheetFillsWindow or zoomSettings.zoomingType == ZoomingType::CUSTOM)
  {
    rect.setSize(scaledSize);
  }
  else if (zoomSettings.zoomingType == ZoomingType::FITSHEET)
  {
    double ratio, targetRatio;
    ratio = scaledSize.height() / scaledSize.width();

    if (sizeSettings.sizeUnit == SizeUnit::CENTIMETER)
      targetRatio = sizeSettings.cmSheetSize.height() / sizeSettings.cmSheetSize.width();
    else
      targetRatio = double(sizeSettings.pxSheetSize.height())
                    / double(sizeSettings.pxSheetSize.width());

    if (ratio <= targetRatio)
    {
      rect.setHeight(scaledSize.height());
      rect.setWidth(scaledSize.height() / targetRatio);

      rect.moveTopLeft(QPointF((scaledSize.width() - rect.width()) / 2, 0));
    }
    else
    {
      rect.setHeight(scaledSize.width() * targetRatio);
      rect.setWidth(scaledSize.width());

      rect.moveTopLeft(QPoint(0, (scaledSize.height() - rect.height()) / 2));
    }
  }

  qDebug() << "Available rect: " << information.getAvailableSheetSizePx();
  qDebug() << "Scaled size: " << scaledSize;
  qDebug() << "Support rect: " << rect;

  supportRect = rect.toRect();
}

void Graph::updateFigureSize()
{
  sizeSettings.cmFigureSize.setWidth(relFigRect.width() * sizeSettings.cmSheetSize.width());
  sizeSettings.cmFigureSize.setHeight(relFigRect.height() * sizeSettings.cmSheetSize.height());

  sizeSettings.pxFigureSize.setWidth(
    int(relFigRect.width() * double(sizeSettings.pxSheetSize.width())));
  sizeSettings.pxFigureSize.setHeight(
    int(relFigRect.height() * double(sizeSettings.pxSheetSize.height())));
}

void Graph::onZoomSettingsChange()
{
  qDebug() << "graph: updating cached zoom settings";
  zoomSettings = information.getGraphZoomSettings();
  updateFigureSize();
  update();
}

void Graph::onSizeSettingsChange()
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
    if (sizeSettings.sizeUnit == SizeUnit::CENTIMETER)
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

void Graph::constrainFigureRectRel()
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

void Graph::exportPDF(QString fileName, SheetSizeType sizeType)
{
  QPdfWriter *pdfWriter = new QPdfWriter(fileName);

  pdfWriter->setCreator(QString("ZeGrapher ") + SOFTWARE_VERSION);
  pdfWriter->setTitle(tr("Exported graph"));

  int targetResolution = int(information.getPixelDensity() / sizeSettings.scalingFactor);

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

  figureRectScaled = getFigureRect(supportRect);

  painter->translate(figureRectScaled.topLeft());

  drawAll();

  painter->end();

  delete pdfWriter;
}

void Graph::exportSVG(QString fileName)
{
  QSvgGenerator svgGenerator;
  svgGenerator.setFileName(fileName);

  svgGenerator.setTitle(tr("Exported graph"));
  svgGenerator.setDescription(tr("Created with ZeGrapher ") + SOFTWARE_VERSION);

  double targetResolution = information.getPixelDensity() / sizeSettings.scalingFactor;

  svgGenerator.setResolution(int(targetResolution));

  QSize sizePx(int(sizeSettings.cmSheetSize.width() * 0.393701 * targetResolution),
               int(sizeSettings.cmSheetSize.height() * 0.393701 * targetResolution));

  svgGenerator.setSize(sizePx);
  svgGenerator.setViewBox(QRect(QPoint(0, 0), sizePx));

  painter->begin(&svgGenerator);

  if (information.getGraphSettings().backgroundColor != QColor(Qt::white))
  {
    painter->setBrush(QBrush(information.getGraphSettings().backgroundColor));
    painter->drawRect(painter->window());
  }

  figureRectScaled = getFigureRect(painter->window());

  painter->translate(figureRectScaled.topLeft());

  drawAll();

  painter->end();
}
