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

#include "GraphDraw/mathobjectdraw.h"

#include <cmath>

using namespace std;

MathObjectDraw::MathObjectDraw()
  : sampler(viewMapper)
{
  pen.setCapStyle(Qt::RoundCap);
  brush.setStyle(Qt::SolidPattern);
}

void MathObjectDraw::drawDataPoint(const QPointF& pt, const zg::CurveStyle& style)
{
  double w = style.pointWidth;

  pen.setColor(style.color);
  pen.setWidthF(style.lineWidth);
  pen.setCapStyle(Qt::PenCapStyle::RoundCap);
  pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
  brush.setColor(style.color);
  painter->setBrush(brush);
  painter->setPen(pen);

  painter->setRenderHint(QPainter::Antialiasing, true);

  switch (style.pointStyle)
  {
  case zg::PlotStyle::Cross:
  {
    painter->drawLine(pt + QPointF(0, w), pt + QPointF(0, -w));
    painter->drawLine(pt + QPointF(-w, 0), pt + QPointF(w, 0));
    break;
  }
  case zg::PlotStyle::Disc:
    painter->drawEllipse(pt, w, w);
    break;
  case zg::PlotStyle::Rhombus:
  {
    QPolygonF polygon(
      {pt + QPointF(-w, 0), pt + QPointF(0, w), pt + QPointF(w, 0), pt + QPointF(0, -w)});

    painter->drawPolygon(polygon);
    break;
  }
  case zg::PlotStyle::Square:
  {
    QRectF rect;
    rect.setTopLeft(pt + QPointF(-w, -w));
    rect.setBottomRight(pt + QPointF(w, w));

    painter->drawRect(rect);
    break;
  }
  case zg::PlotStyle::Triangle:
  {
    QPolygonF polygon;
    constexpr double coef = 0.86602540378443864676; // sqrt(3) / 2;
    double d = w * coef;
    double b = w / 2.;

    polygon << pt + QPointF(0, -w) << pt + QPointF(d, b) << pt + QPointF(-d, b);

    painter->drawPolygon(polygon);
    break;
  }
  case zg::PlotStyle::None:
    break;
  }
}

void MathObjectDraw::drawObjects()
{
  for (const auto& f_curve: sampler.getCurves())
    drawSampledCurve(f_curve);
}

QList<QPolygonF> buildFinalCurve(const zg::SampledCurve& sampledCurve)
{
  QList<QPolygonF> finalCurve;
  const auto& curve = sampledCurve.px_curve;

  if (curve.empty())
    return finalCurve;

  /// curve in-between a pair of any of the following:
  // - discontinuity
  // - NaNs
  // - beginning or end of the curve
  QPolygonF currentSection;
  currentSection.reserve(sampledCurve.size());

  auto flush = [&](size_t remainingNumPoints)
  {
    if (not currentSection.empty())
      finalCurve.append(std::move(currentSection));

    // since we std::move'd it, the allocated array got moved  with it: need a new reserve
    currentSection.reserve(remainingNumPoints);
  };

  for (size_t i = 0; i != curve.size(); i++)
  {
    const auto& pt = curve[i];

    if (std::isnan(pt.x()) or std::isnan(pt.y()))
    {
      flush(curve.size() - i);
      continue;
    }

    if (sampledCurve.discontinuities.contains(i))
      flush(curve.size() - i);

    currentSection.append(pt);
  }

  flush(0);

  return finalCurve;
}

void MathObjectDraw::drawSampledCurve(const zg::SampledCurve& f_curve)
{
  if (not f_curve.style.visible)
    return;

  painter->setRenderHint(QPainter::Antialiasing);

  const auto& style = f_curve.style;

  if (style.drawLine)
  {
    pen.setWidthF(style.lineWidth);
    pen.setColor(style.color);
    pen.setCapStyle(Qt::PenCapStyle::RoundCap);
    pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    if (style.dashPattern.isEmpty())
      pen.setStyle(Qt::SolidLine);
    else
      pen.setDashPattern(style.dashPattern);

    painter->setPen(pen);

    const QList<QPolygonF> curve = buildFinalCurve(f_curve);

    for (const QPolygonF& seg : curve)
      painter->drawPolyline(seg);
  }

  if (f_curve.discrete)
  {
    if (style.pointStyle != zg::PlotStyle::None)
      for (const QPointF& P: f_curve.px_curve)
      {
        if (not std::isnan(P.x()) and not std::isnan(P.y()))
          drawDataPoint(P, style);
      }
  }
}
