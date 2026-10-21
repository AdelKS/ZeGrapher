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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Utils/plotstyle.h"

#include <cmath>

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent)
{
  connect(this, &PlotStyle::visibleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::colorChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::secondColorChanged, this, &PlotStyle::updated);
}

QList<qreal> PlotStyle::getDashPattern() const
{
  switch (getLineStyle())
  {
    case Dash: return {4., 2.};
    case DashDot: return {4., 2., 2., 2.};
    case Dot: return {1., 2.};
    case NoLine: return {};
    case Solid: return {};
  }
  return {};
}

QColor PlotStyle::colorLerp(double t) const
{
  const QColor &a = color.getCurrent(), &b = secondColor.getCurrent();
  return QColor::fromRgbF(
      std::lerp(a.redF(), b.redF(), t),
      std::lerp(a.greenF(), b.greenF(), t),
      std::lerp(a.blueF(), b.blueF(), t));
}

void PlotStyle::setVisible(bool v)
{
  if (visible != v)
  {
    visible = v;
    emit visibleChanged();
  }
}

PlotStyle::LineStyle PlotStyle::defaultLineStyle() const
{
  if (discrete)
    return LineStyle::NoLine;
  else return LineStyle::Solid;
}

PlotStyle::PointStyle PlotStyle::defaultPointStyle() const
{
  if (discrete)
    return PointStyle::Square;
  else return PointStyle::None;
}

void PlotStyle::setDiscrete(bool d)
{
  if (d == discrete) return;

  const LineStyle oldLineStyle = getLineStyle();
  const PointStyle oldPointStyle = getPointStyle();

  discrete = d;

  if (getLineStyle() != oldLineStyle)
    emit lineStyleChanged();

  if (getPointStyle() != oldPointStyle)
    emit pointStyleChanged();
}

void PlotStyle::setLineWidth(double s)
{
  if (lineWidth == s) return;
  lineWidth = s;
  emit lineWidthChanged();
}

void PlotStyle::setLineStyle(LineStyle s)
{
  if (getLineStyle() == s) return;
  // picking the default of the object's kind follows that kind again
  lineStyle = s == defaultLineStyle() ? std::nullopt : std::optional(s);
  emit lineStyleChanged();
}

void PlotStyle::setPointStyle(PointStyle s)
{
  if (getPointStyle() == s) return;
  pointStyle = s == defaultPointStyle() ? std::nullopt : std::optional(s);
  emit pointStyleChanged();
}

void PlotStyle::setPointWidth(double s)
{
  if (pointWidth == s) return;
  pointWidth = s;
  emit pointWidthChanged();
}

std::optional<PlotStyle::POD> PlotStyle::exportPod() const
{
  return yml::if_any(POD {
    .visible = yml::not_default(visible, true),
    .color = color.exportPod(defaultColor),
    .second_color = secondColor.exportPod(defaultSecondColor),
    .line_width = yml::not_default(lineWidth, defaultLineWidth),
    .line_style = lineStyle,
    .point_width = yml::not_default(pointWidth, defaultPointWidth),
    .point_style = pointStyle,
  });
}

void PlotStyle::importPod(POD p)
{
  if (p.visible)
    setVisible(*p.visible);

  if (p.color)
  {
    color.importPod(std::move(*p.color));
    emit colorChanged();
  }

  if (p.second_color)
  {
    secondColor.importPod(std::move(*p.second_color));
    emit secondColorChanged();
  }

  if (p.line_width)
    setLineWidth(yml::clamped(*p.line_width, minLineWidth, maxLineWidth));

  // the object's kind can change once the objects it uses load, so keep the
  // style even when it is the current default
  if (p.line_style)
  {
    lineStyle = *p.line_style;
    emit lineStyleChanged();
  }

  if (p.point_width)
    setPointWidth(yml::clamped(*p.point_width, minPointWidth, maxPointWidth));

  if (p.point_style)
  {
    pointStyle = *p.point_style;
    emit pointStyleChanged();
  }
}

}
