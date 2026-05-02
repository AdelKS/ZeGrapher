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
#include "information.h"

using namespace std;

MathObjectDraw::MathObjectDraw(double targetSamplingDistance)
  : sampler(viewMapper, targetSamplingDistance)
{
  coef = sqrt(3) / 2;

  pen.setCapStyle(Qt::RoundCap);
  brush.setStyle(Qt::SolidPattern);

  moving = false;
}

void MathObjectDraw::drawObjects()
{
  const auto start = std::chrono::high_resolution_clock::now();

  for (const auto& f_curve: sampler.getCurves())
    drawSampledCurve(f_curve);

  const auto end = std::chrono::high_resolution_clock::now();

  qDebug() << "Plotting curves took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

QList<QPolygonF> buildFinalCurve(const zg::SampledCurve& sampledCurve, double scale)
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

    currentSection.append(pt * scale);
  }

  flush(0);

  return finalCurve;
}

QList<QPolygonF> buildMarkerPaths(const zg::SampledCurve& curve, double scale)
{
  QList<QPolygonF> markers;

  if (curve.style.pointStyle == zg::CurveStyle::None)
    return markers;

  const double w = curve.style.pointWidth / 2;
  markers.reserve(curve.style.pointStyle == zg::CurveStyle::Cross ? curve.size() * 2 : curve.size());

  for (const QPointF& raw : curve.px_curve)
  {
    if (std::isnan(raw.x()) or std::isnan(raw.y()))
      continue;
    const QPointF p = raw * scale;

    switch (curve.style.pointStyle)
    {
    case zg::CurveStyle::Cross:
      markers.append(QPolygonF({p + QPointF(-2 * w, 0), p + QPointF(2 * w, 0)}));
      markers.append(QPolygonF({p + QPointF(0, -2 * w), p + QPointF(0, 2 * w)}));
      break;
    case zg::CurveStyle::Disc:
    {
      constexpr int N = 16;
      QPolygonF poly;
      poly.reserve(N + 1);
      for (int i = 0; i <= N; ++i)
      {
        const double a = 2. * std::numbers::pi * i / N;
        poly << p + QPointF(w * std::cos(a), w * std::sin(a));
      }
      markers.append(std::move(poly));
      break;
    }
    case zg::CurveStyle::Rhombus:
    {
      double W = 1.5*w;
      markers.append(QPolygonF({p + QPointF(-W, 0), p + QPointF(0, W),
                                p + QPointF(W, 0),  p + QPointF(0, -W),
                                p + QPointF(-W, 0)}));
      break;
    }
    case zg::CurveStyle::Square:
      markers.append(QPolygonF({p + QPointF(-w, -w), p + QPointF(w, -w),
                                p + QPointF(w, w),   p + QPointF(-w, w),
                                p + QPointF(-w, -w)}));
      break;
    case zg::CurveStyle::Triangle:
    {
      const double W = 1.5 * w;
      const double d = W * std::numbers::sqrt3 / 2.;
      const double b = W / 2;
      markers.append(QPolygonF({p + QPointF(0, -W), p + QPointF(d, b),
                                p + QPointF(-d, b), p + QPointF(0, -W)}));
      break;
    }
    case zg::CurveStyle::None:
      break;
    }
  }
  return markers;
}


void MathObjectDraw::drawSampledCurve(const zg::SampledCurve& f_curve)
{
  if (not f_curve.style.visible)
    return;

  painter->setRenderHint(QPainter::Antialiasing);

  const auto& style = f_curve.style;

  if (style.drawLine)
  {
    pen.setWidth(style.lineWidth);
    pen.setColor(style.color);
    if (style.dashPattern.isEmpty())
      pen.setStyle(Qt::SolidLine);
    else
      pen.setDashPattern(style.dashPattern);

    painter->setPen(pen);

    const QList<QPolygonF> curve = buildFinalCurve(f_curve);

    size_t plotted_points = 0;
    for (const QPolygonF& seg : curve)
    {
      painter->drawPolyline(seg);
      plotted_points += seg.size();
    }

    qDebug() << "Plotted continuous curve with" << plotted_points << "points.";

  }

  if (f_curve.discrete and style.pointStyle != zg::CurveStyle::PointStyle::None)
  {
    const bool crossMarker = (style.pointStyle == zg::CurveStyle::Cross);

    pen.setColor(style.color);
    pen.setWidth(crossMarker ? style.lineWidth : style.pointWidth);
    pen.setStyle(Qt::SolidLine);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    if (crossMarker)
      brush.setStyle(Qt::NoBrush);
    else
    {
      brush.setColor(style.color);
      brush.setStyle(Qt::SolidPattern);
    }

    painter->setPen(pen);
    painter->setBrush(brush);

    QList<QPolygonF> markers = buildMarkerPaths(f_curve);
    for (const QPolygonF& poly: markers)
    {
      if (crossMarker)
        painter->drawPolyline(poly);
      else
        painter->drawPolygon(poly);
    }
  }
}
