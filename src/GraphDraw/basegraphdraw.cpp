/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "GraphDraw/basegraphdraw.h"
#include "information.h"

BaseGraphDraw::BaseGraphDraw()
  : MathObjectDraw(), gridCalculator(this), fontMetrics(information.getGraphSettings().graphFont)
{
  leftMargin = 30;
  rightMargin = 30;
  topMargin = 20;
  bottomMargin = 30;

  legendFontSize = 12;
  legendState = false;
  additionalMargin = 0;
  bold = italic = underline = false;
  numPrec = NUM_PREC;
  viewMapper.setGraphRange(information.getGraphRange());

  connect(&information,
          SIGNAL(graphRangeChanged(GraphRange)),
          this,
          SLOT(graphRangeChanged(const GraphRange &)));
  connect(&information, SIGNAL(estheticSettingsChanged()), this, SLOT(update()));
  connect(&information, SIGNAL(updateOccured()), this, SLOT(update()));
}

void BaseGraphDraw::graphRangeChanged(const GraphRange &range)
{
  viewMapper.setGraphRange(range);
  update();
}

void BaseGraphDraw::setNumPrec(int prec)
{
  numPrec = prec;
  update();
}

void BaseGraphDraw::setBold(bool state)
{
  bold = state;
  update();
}

void BaseGraphDraw::setUnderline(bool state)
{
  underline = state;
  update();
}

void BaseGraphDraw::setItalic(bool state)
{
  italic = state;
  update();
}

void BaseGraphDraw::setlegendFontSize(int size)
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

void BaseGraphDraw::setLegendState(bool show)
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

void BaseGraphDraw::setXaxisLegend(QString legend)
{
  xLegend = legend;
  update();
}

void BaseGraphDraw::setYaxisLegend(QString legend)
{
  yLegend = legend;
  update();
}

void BaseGraphDraw::updateGraphRect()
{
  graphRectScaled.setWidth(figureRectScaled.width() - leftMargin - rightMargin);
  graphRectScaled.setHeight(figureRectScaled.height() - topMargin - bottomMargin);
  graphRectScaled.moveTopLeft(QPoint(0, 0)); // because painter is translated to its top-left corner
  viewMapper.setGraphRect(graphRectScaled);
}

void BaseGraphDraw::calculateTicksAndMargins()
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

void BaseGraphDraw::drawAll()
{
  funcValuesSaver.update();
  painter->setFont(information.getGraphSettings().graphFont);
  fontMetrics = painter->fontMetrics();

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

  funcValuesSaver.update();
  drawFunctions();

  painter->translate(QPointF(centre.x, centre.y));

  recalculateRegVals();
  drawSequences();
  drawStaticParEq();
  drawRegressions();
  drawData();
}

void BaseGraphDraw::writeLegends()
{
  font = information.getGraphSettings().graphFont;
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

void BaseGraphDraw::writeAxisOffsetX()
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

void BaseGraphDraw::writeAxisOffsetY()
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

void BaseGraphDraw::drawGraphRect()
{
  const auto &axesSettings = information.getAxesSettings();

  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->setBrush(QBrush(Qt::NoBrush));

  pen.setWidth(axesSettings.lineWidth);
  pen.setColor(axesSettings.color);
  painter->setPen(pen);

  painter->drawRect(graphRectScaled);
}

void BaseGraphDraw::updateCenterPosAndScaling()
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

QImage *BaseGraphDraw::drawImage()
{
  QImage *image = new QImage(size(), QImage::Format_RGB32);
  image->fill(information.getGraphSettings().backgroundColor.rgb());

  painter->begin(image);
  //trace du background

  drawAll();

  painter->end();

  //*image = image->convertToFormat(QImage::Format_Indexed8, Qt::DiffuseDither);
  return image;
}
