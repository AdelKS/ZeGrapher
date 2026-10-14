/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#pragma once

#include <QColor>
#include <QFont>
#include <QLineF>
#include <QMarginsF>
#include <QRectF>

#include <ranges>
#include <vector>

#include "gridcalculator.h"
#include "viewmapper.h"

/// @brief straight lines of one color and width
struct LineBatch
{
  QColor color;
  double width = 1;
  Qt::PenCapStyle cap = Qt::FlatCap;
  std::vector<QLineF> lines = {};
};

enum class TextRole { Tick, Title };

/// @brief a text whose baseline starts at 'origin', turned by 'angle' degrees
///        around it
struct Label
{
  QString text;
  TextRole role = TextRole::Tick;
  QPointF origin;
  double angle = 0;
};

/// @brief everything around a graph's curves: the grid, axes, ticks, border
///        and texts
///
/// Everything is in drawing units, 1 / ZeGraphSettings::targetPixelDensity cm
/// at the 1x scale. Lines and labels are relative to the top left corner of
/// 'graph', and the curves go inside 'graph'.
struct GraphLayout
{
  QRectF figure, graph;

  /// @brief the subgrid, the grid and the axes, which the curves cover
  std::vector<LineBatch> belowCurves;

  /// @brief the ticks, which cover the curves
  std::vector<LineBatch> aboveCurves;

  LineBatch border;
  std::vector<Label> labels;

  /// @brief for a 72 dpi device
  QFont tickFont, titleFont;
  QColor textColor;

  const QFont& font(TextRole role) const { return role == TextRole::Title ? titleFont : tickFont; }
};

/// @brief lays a graph out from its settings
///
/// Text is measured on a 72 dpi device with hinting off, so the layout is the
/// same on every screen and in every file.
class GraphLayoutEngine
{
public:
  GraphLayoutEngine(ZeGraphSettings& settings, zg::ZeViewMapper& viewMapper);

  /// @brief lays the graph out in 'figure', and gives the view mapper the
  ///        graph rect
  GraphLayout compute(const QRectF& figure);

  /// @brief a 72 dpi device, where one font point is one drawing unit
  static const QPaintDevice* referenceDevice();

protected:
  template <ZeAxisName axis>
  struct AxisData
  {
    const zg::ZeAxisMapper<axis>& mapper;
    const std::vector<ZeLinAxisTick>& ticks;
    const Ze1DGridSettings& grid;
    const Ze1DSubgridSettings& subgrid;

    zg::pixel_unit toPixel(zg::real_unit pos) const { return mapper.template to<zg::pixel>(pos); }

    /// @brief the ticks strictly inside the view
    auto visibleTicks() const
    {
      return ticks | std::views::filter([range = mapper.template getRange<zg::real>()](const ZeLinAxisTick& tick) {
        return range.strictlyContains(tick.pos);
      });
    }
  };

  template <ZeAxisName axis>
  AxisData<axis> axisData() const;

  QFont font(TextRole role) const;
  void computeTicksAndMargins();
  void setGraphRect();

  template <ZeAxisName axis>
  void addSubgrid();

  template <ZeAxisName axis>
  void addGrid();

  template <ZeAxisName axis>
  void addAxis();

  template <ZeAxisName axis>
  void addTicks();

  template <ZeAxisName axis>
  void addCoordinates();

  void addOffsets();
  void addTitles();

  /// @brief a line across the whole graph, perpendicular to 'axis' at 'pos'
  template <ZeAxisName axis>
  QLineF across(zg::pixel_unit pos) const;

  ZeGraphSettings& settings;
  zg::ZeViewMapper& viewMapper;
  GridCalculator gridCalculator;

  TextMetrics metrics, titleMetrics;
  ZeLinAxisTicks xAxisTicks, yAxisTicks;
  QMarginsF margins{30, 20, 30, 30};

  /// @brief the layout that compute() builds
  GraphLayout layout;
};

template <ZeAxisName axis>
GraphLayoutEngine::AxisData<axis> GraphLayoutEngine::axisData() const
{
  if constexpr (axis == ZeAxisName::X)
    return {viewMapper.x, xAxisTicks.ticks, settings.getGrid().x, settings.getSubgrid().x};
  else return {viewMapper.y, yAxisTicks.ticks, settings.getGrid().y, settings.getSubgrid().y};
}

template <ZeAxisName axis>
QLineF GraphLayoutEngine::across(zg::pixel_unit pos) const
{
  if constexpr (axis == ZeAxisName::X)
    return QLineF(pos.v, 0., pos.v, layout.graph.height());
  else return QLineF(0., pos.v, layout.graph.width(), pos.v);
}
