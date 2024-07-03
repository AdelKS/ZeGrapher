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

#pragma once

#include "gridcalculator.h"
#include "mathobjectdraw.h"

class BaseGraphDraw : public MathObjectDraw
{
  Q_OBJECT
public:
  explicit BaseGraphDraw();
  QImage* drawImage();

public slots:
  void setlegendFontSize(int size);
  void setLegendState(bool show);

  void setXaxisLegend(QString legend);
  void setYaxisLegend(QString legend);

  void setBold(bool state);
  void setUnderline(bool state);
  void setItalic(bool state);
  void setNumPrec(int prec);

  void graphRangeChanged(const GraphRange& range);

protected slots:
  void addRegSaver(Regression *reg);
  void delRegSaver(Regression *reg);
  void updateSettingsVals();

protected:
  void updateCenterPosAndScaling();
  void drawBaseGraph();
  void drawGraphRect();
  void drawAll();
  void updateGraphRect();
  void writeLegends();

  /// @brief draw a line that spans the whole graph width or height
  /// @tparam axis: the line will be perpendicular to this axis
  /// @param pos: the lines intersects 'axis' at position 'pos'
  /// @param col: color of the line
  /// @param lineWidth: width of the line
  template <ZeAxisName axis>
  void drawLine(zg::pixel_unit pos, const QColor& col, double lineWidth);

  /// @brief draws a "tick" on 'axis' at position 'pos'
  /// @tparam axis: the line will be perpendicular to this axis
  /// @param pos: position of the 'tick'
  /// @param col: color of the 'tick'
  /// @param lineWidth: width of the 'tick'
  template <ZeAxisName axis>
  void drawTick(zg::pixel_unit pos, const QColor& col, double lineWidth);

  /// @brief either writes and abscissa or an ordinate next to 'axis'
  template <ZeAxisName axis>
  void writeCoordinate(zg::pixel_unit pos, const QString& txt);

  void writeAxisOffsetX();

  template <ZeAxisName axis>
  void drawLinAxisGridTicks();

  void updateMarginsForAxisOffsetY();
  void writeAxisOffsetY();


  void calculateTicksAndMargins();

  GridCalculator gridCalculator;
  QFontMetrics fontMetrics;
  ZeLinAxisTicks xAxisTicks, yAxisTicks;
  int leftMargin, rightMargin, topMargin, bottomMargin, additionalMargin;
  int  legendFontSize, numPrec;
  QRect figureRectScaled, graphRectScaled;
  QString xLegend, yLegend;
  bool legendState, bold, italic, underline;
};

template <ZeAxisName axis>
void BaseGraphDraw::drawLine(zg::pixel_unit pos, const QColor& col, double lineWidth)
{
  pen.setColor(col);
  pen.setWidthF(lineWidth);
  painter->setPen(pen);
  painter->setRenderHint(QPainter::Antialiasing, false);

  if constexpr (axis == ZeAxisName::X)
    painter->drawLine(QPointF(pos.v, 0.), QPointF(pos.v, graphRectScaled.height()));
  else
    painter->drawLine(QPointF(0., pos.v), QPointF(graphRectScaled.width(), pos.v));
}

template <ZeAxisName axis>
void BaseGraphDraw::drawTick(zg::pixel_unit pos, const QColor& col, double lineWidth)
{
  pen.setColor(col);
  pen.setWidthF(lineWidth);
  painter->setPen(pen);
  painter->setRenderHint(QPainter::Antialiasing, false);

  if constexpr (axis == ZeAxisName::X)
    painter->drawLine(QPointF(pos.v, 4), QPointF(pos.v, 0));
  else
    painter->drawLine(QPointF(4, pos.v), QPointF(0, pos.v));
};

template <ZeAxisName axis>
void BaseGraphDraw::writeCoordinate(zg::pixel_unit pos, const QString& txt)
{
  painter->setFont(information.getGraphSettings().graphFont);
  pen.setColor(information.getAxesSettings().color);
  painter->setPen(pen);

  auto boundingRect = fontMetrics.boundingRect(txt);
  if constexpr (axis == ZeAxisName::X)
  {
    double space;
    if (txt.startsWith('-'))
      space = double(fontMetrics.boundingRect(txt.mid(1)).width()) / 2.
              + fontMetrics.boundingRect('-').width();
    else
      space = double(boundingRect.width()) / 2.;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawText(QPointF(pos.v - space, graphRectScaled.height() + boundingRect.height() + 5), txt);
  }
  else
  {
    painter->drawText(QPointF(- boundingRect.width() - 5, pos.v + boundingRect.height()/2.), txt);
  }
};

template <ZeAxisName axis>
void BaseGraphDraw::drawLinAxisGridTicks()
{
  painter->setFont(information.getGraphSettings().graphFont);
  QFontMetrics fontMetrics = painter->fontMetrics();

  const ZeAxesSettings &axesSettings = information.getAxesSettings();

  pen.setCapStyle(Qt::FlatCap);
  bool first_tick = true;
  zg::pixel_unit previous_pos;

  auto getAxisData = [&]()
  {
    if constexpr (axis == ZeAxisName::X)
      return std::tie(std::as_const(viewMapper.x), xAxisTicks.ticks, information.getGridSettings().x);
    else return std::tie(std::as_const(viewMapper.y), yAxisTicks.ticks, information.getGridSettings().y);
  };
  const auto& [axisMapper, axisTicks, gridSettings] = getAxisData();

  const zg::pixel_unit zero_pt = axisMapper.template to<zg::pixel>(zg::real_unit{0.});

  for (const ZeLinAxisTick &axisTick : axisTicks)
  {
    if (not(axisMapper.template getRange<zg::real>().min < axisTick.pos
            && axisTick.pos < axisMapper.template getRange<zg::real>().max))
      continue;

    zg::pixel_unit px_pos = axisMapper.template to<zg::pixel>(axisTick.pos);

    if (fabs(px_pos.v - zero_pt.v) > 1.)
    {
      if (gridSettings.showGrid)
        drawLine<axis>(px_pos, gridSettings.gridColor, gridSettings.gridLineWidth);

      drawTick<axis>(px_pos, axesSettings.color, axesSettings.lineWidth);
      writeCoordinate<axis>(px_pos, axisTick.posStr);
    }
    else
    {
      drawLine<axis>(zero_pt, axesSettings.color, axesSettings.lineWidth);
      writeCoordinate<axis>(zero_pt, "0");
    }

    if (gridSettings.showSubGrid && !first_tick)
    {
      for (uint mul = 1; mul <= gridSettings.subgridSubDivs; mul++)
      {
        const zg::pixel_unit cur_pos = double(mul) / double(gridSettings.subgridSubDivs + 1)
                                         * previous_pos
                                       + double(gridSettings.subgridSubDivs + 1 - mul)
                                           / double(gridSettings.subgridSubDivs + 1) * px_pos;

        if ( 0 < cur_pos.v && cur_pos.v < graphRectScaled.width())
          drawLine<axis>(cur_pos, gridSettings.subgridColor, gridSettings.subgridLineWidth);
      }
    }

    previous_pos = px_pos;
    first_tick = false;
  }
}
