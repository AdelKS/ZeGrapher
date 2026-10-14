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

#include "GraphDraw/graphlayout.h"

#include <QPaintDevice>

#include <cmath>
#include <utility>

namespace {

class ReferenceDevice : public QPaintDevice
{
public:
  QPaintEngine* paintEngine() const override { return nullptr; }

protected:
  int metric(PaintDeviceMetric m) const override
  {
    switch (m)
    {
    case PdmDpiX:
    case PdmDpiY:
    case PdmPhysicalDpiX:
    case PdmPhysicalDpiY:
      return 72;
    case PdmDevicePixelRatio:
      return 1;
    case PdmDevicePixelRatioScaled:
      return int(devicePixelRatioFScale());
    case PdmDepth:
      return 32;
    case PdmNumColors:
    case PdmWidth:
    case PdmHeight:
    case PdmWidthMM:
    case PdmHeightMM:
    case PdmDevicePixelRatioF_EncodedA:
    case PdmDevicePixelRatioF_EncodedB:
      return 0;
    }
    return 0;
  }
};

}

GraphLayoutEngine::GraphLayoutEngine(ZeGraphSettings& settings, zg::ZeViewMapper& viewMapper)
  : settings(settings),
    viewMapper(viewMapper),
    gridCalculator(settings),
    metrics(font(TextRole::Tick), referenceDevice()),
    titleMetrics(font(TextRole::Title), referenceDevice())
{}

const QPaintDevice* GraphLayoutEngine::referenceDevice()
{
  static const ReferenceDevice device;
  return &device;
}

QFont GraphLayoutEngine::font(TextRole role) const
{
  QFont f = role == TextRole::Title ? settings.getAxes().titleFont : settings.getFont();
  f.setPointSizeF(f.pointSizeF() * ZeGraphSettings::targetPixelDensity * CM_PER_INCH / 72.);
  f.setHintingPreference(QFont::PreferNoHinting);
  return f;
}

void GraphLayoutEngine::setGraphRect()
{
  layout.graph = layout.figure.marginsRemoved(margins);
  viewMapper.setGraphRect(QRectF(QPointF(), layout.graph.size()));
}

void GraphLayoutEngine::computeTicksAndMargins()
{
  setGraphRect();

  xAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.x, metrics);
  yAxisTicks = gridCalculator.getLinearAxisTicks(viewMapper.y, metrics);

  if (xAxisTicks.offset.sumOffset != 0)
  {
    double margin = metrics.boundingRect(xAxisTicks.offset.sumOffsetStr()).width() + 5;
    if (margins.right() < margin)
      margins.setRight(10 + margin);
  }

  if (xAxisTicks.offset.basePowerOffset != 0)
  {
    double margin = metrics.boundingRect(xAxisTicks.offset.basePowerOffsetStr()).width() + 5;
    margins.setRight(10 + margin);
  }

  if (xAxisTicks.offset.basePowerOffset == 0 and xAxisTicks.offset.sumOffset == 0)
    margins.setRight(10);

  double offsetMargin = 0;
  if (yAxisTicks.offset.basePowerOffset != 0)
    offsetMargin = metrics.boundingRect(yAxisTicks.offset.basePowerOffsetStr()).height();

  if (yAxisTicks.offset.sumOffset != 0)
    offsetMargin = std::max(offsetMargin,
                            metrics.boundingRect(yAxisTicks.offset.sumOffsetStr()).height());

  const double gap = settings.getAxes().spacing.v;

  double xLegendMargin = 0, yLegendMargin = 0;
  if (not settings.getAxes().y.title.isEmpty())
    yLegendMargin = titleMetrics.tightBoundingRect(settings.getAxes().y.title).height() + gap;

  if (not settings.getAxes().x.title.isEmpty())
    xLegendMargin = titleMetrics.tightBoundingRect(settings.getAxes().x.title).height() + gap;

  // layout: graph edge | gap | coordinates | gap | title | gap | figure edge
  margins.setBottom(gap + xAxisTicks.maxPxHeight() + xLegendMargin + gap);
  margins.setLeft(gap + yAxisTicks.maxPxWidth + yLegendMargin + gap);
  margins.setTop(std::max(20., 5. + offsetMargin));

  setGraphRect();
}

GraphLayout GraphLayoutEngine::compute(const QRectF& figure)
{
  layout.figure = figure;
  layout.tickFont = font(TextRole::Tick);
  layout.titleFont = font(TextRole::Title);
  metrics = QFontMetricsF(layout.tickFont, referenceDevice());
  titleMetrics = QFontMetricsF(layout.titleFont, referenceDevice());
  viewMapper.setGraphRange(settings.range.getLatestValidSnapshot());

  // the margins hold the tick labels, and the labels depend on the room that
  // the margins leave to the graph
  for (int pass = 0; pass != 3; pass++)
  {
    const QMarginsF before = margins;
    computeTicksAndMargins();
    if (margins == before)
      break;
  }

  const ZeAxesSettings& axes = settings.getAxes();
  layout.textColor = axes.color.getCurrent();

  if (axes.y.axisType == ZeAxisSettings::LINEAR and axes.x.axisType == ZeAxisSettings::LINEAR)
  {
    addSubgrid<ZeAxisName::Y>();
    addSubgrid<ZeAxisName::X>();

    addGrid<ZeAxisName::Y>();
    addGrid<ZeAxisName::X>();

    addAxis<ZeAxisName::Y>();
    addAxis<ZeAxisName::X>();

    addTicks<ZeAxisName::Y>();
    addTicks<ZeAxisName::X>();

    addOffsets();

    addCoordinates<ZeAxisName::Y>();
    addCoordinates<ZeAxisName::X>();
  }

  const double w = layout.graph.width(), h = layout.graph.height();
  layout.border = LineBatch{
    .color = axes.color.getCurrent(),
    .width = axes.lineWidth,
    .cap = Qt::RoundCap,
    .lines = {QLineF(0, 0, w, 0), QLineF(w, 0, w, h), QLineF(w, h, 0, h), QLineF(0, h, 0, 0)},
  };

  addTitles();

  return std::exchange(layout, {});
}

template <ZeAxisName axis>
void GraphLayoutEngine::addSubgrid()
{
  const auto data = axisData<axis>();
  if (not data.subgrid.show)
    return;

  LineBatch batch{.color = data.subgrid.color.getCurrent(), .width = data.subgrid.lineWidth};

  const auto range = data.mapper.template getRange<zg::real>();
  const double parts = double(data.subgrid.subdivs + 1);

  for (const auto& [previous, next] : std::views::zip(data.ticks, data.ticks | std::views::drop(1)))
    for (int mul = 1; mul <= data.subgrid.subdivs; mul++)
    {
      const zg::real_unit pos{std::lerp(next.pos.v, previous.pos.v, mul / parts)};
      if (range.strictlyContains(pos))
        batch.lines.push_back(across<axis>(data.toPixel(pos)));
    }

  layout.belowCurves.push_back(std::move(batch));
}

template <ZeAxisName axis>
void GraphLayoutEngine::addGrid()
{
  const auto data = axisData<axis>();
  if (not data.grid.show)
    return;

  LineBatch batch{.color = data.grid.color.getCurrent(), .width = data.grid.lineWidth};

  const zg::pixel_unit zero = data.toPixel(zg::real_unit{0.});

  for (const ZeLinAxisTick& tick : data.visibleTicks())
  {
    const zg::pixel_unit pos = data.toPixel(tick.pos);

    // the axis covers the grid line at zero
    if (std::abs(pos.v - zero.v) > 1.)
      batch.lines.push_back(across<axis>(pos));
  }

  layout.belowCurves.push_back(std::move(batch));
}

template <ZeAxisName axis>
void GraphLayoutEngine::addAxis()
{
  const auto data = axisData<axis>();
  const zg::pixel_unit zero = data.toPixel(zg::real_unit{0.});

  if (data.mapper.isInView(zero))
    layout.belowCurves.push_back(LineBatch{.color = settings.getAxes().color.getCurrent(),
                                           .width = settings.getAxes().lineWidth,
                                           .lines = {across<axis>(zero)}});
}

template <ZeAxisName axis>
void GraphLayoutEngine::addTicks()
{
  const auto data = axisData<axis>();
  constexpr double tickLength = 3;

  LineBatch batch{.color = settings.getAxes().color.getCurrent(),
                  .width = settings.getAxes().lineWidth,
                  .cap = Qt::RoundCap};

  const double zero = data.toPixel(zg::real_unit{0.}).v;
  const double w = layout.graph.width(), h = layout.graph.height();

  for (const ZeLinAxisTick& tick : data.visibleTicks())
  {
    const double pos = data.toPixel(tick.pos).v;

    if (std::abs(pos - zero) <= 1.)
      continue;

    if constexpr (axis == ZeAxisName::X)
    {
      batch.lines.emplace_back(pos, tickLength, pos, 0);
      batch.lines.emplace_back(pos, h - tickLength, pos, h);
    }
    else
    {
      batch.lines.emplace_back(tickLength, pos, 0, pos);
      batch.lines.emplace_back(w - tickLength, pos, w, pos);
    }
  }

  layout.aboveCurves.push_back(std::move(batch));
}

template <ZeAxisName axis>
void GraphLayoutEngine::addCoordinates()
{
  const auto data = axisData<axis>();
  const double gap = settings.getAxes().spacing.v;

  for (const ZeLinAxisTick& tick : data.visibleTicks())
  {
    const double pos = data.toPixel(tick.pos).v;
    const QString& txt = tick.posStr;
    const double width = metrics.horizontalAdvance(txt);

    QPointF origin;
    if constexpr (axis == ZeAxisName::X)
    {
      // a negative number is centered on its digits, its minus sign hangs left
      const double space = txt.startsWith('-')
                             ? metrics.horizontalAdvance(txt.mid(1)) / 2. + metrics.horizontalAdvance('-')
                             : width / 2.;
      origin = QPointF(pos - space, layout.graph.height() + gap + xAxisTicks.maxPxAscent);
    }
    else origin = QPointF(- width - gap, pos - metrics.tightBoundingRect(txt).top() / 2.);

    layout.labels.push_back(Label{.text = txt, .origin = origin});
  }
}
void GraphLayoutEngine::addOffsets()
{
  const ZeOffset& x = xAxisTicks.offset;
  const double w = layout.graph.width(), h = layout.graph.height();

  if (x.sumOffset != 0)
  {
    const QString text = x.sumOffsetStr();
    layout.labels.push_back(Label{.text = text,
                                  .origin = QPointF(w + 5, h - metrics.boundingRect(text).height() - 10)});
  }

  if (x.basePowerOffset != 0)
    layout.labels.push_back(Label{.text = x.basePowerOffsetStr(), .origin = QPointF(w + 5, h)});

  const ZeOffset& y = yAxisTicks.offset;
  double powerOffsetWidth = 0;

  if (y.basePowerOffset != 0)
  {
    const QString text = y.basePowerOffsetStr();
    layout.labels.push_back(Label{.text = text, .origin = QPointF(0, -4)});
    powerOffsetWidth = metrics.boundingRect(text).width() + 5;
  }

  if (y.sumOffset != 0)
    layout.labels.push_back(Label{.text = y.sumOffsetStr(), .origin = QPointF(powerOffsetWidth + 5, -4)});
}

void GraphLayoutEngine::addTitles()
{
  const double gap = settings.getAxes().spacing.v;

  if (const QString& title = settings.getAxes().x.title; not title.isEmpty())
  {
    // the ink rect is relative to the start of the baseline: top() < 0
    const QRectF ink = titleMetrics.tightBoundingRect(title);
    layout.labels.push_back(Label{
      .text = title,
      .role = TextRole::Title,
      .origin = QPointF((layout.graph.width() - ink.width()) / 2 - ink.left(),
                        layout.graph.height() + margins.bottom() - gap - ink.bottom()),
    });
  }

  if (const QString& title = settings.getAxes().y.title; not title.isEmpty())
  {
    // laid out along the rotated baseline, then turned back: (x, y) -> (y, -x)
    const QRectF ink = titleMetrics.tightBoundingRect(title);
    const QPointF along(-(layout.graph.height() + ink.width()) / 2 - ink.left(),
                        -margins.left() + gap - ink.top());
    layout.labels.push_back(Label{
      .text = title,
      .role = TextRole::Title,
      .origin = QPointF(along.y(), -along.x()),
      .angle = -90,
    });
  }
}
