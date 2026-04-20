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

void MathObjectDraw::drawDataPoint(const QPointF& pt, const zg::PlotStyle& style)
{
  double w = style.pointWidth;

  pen.setColor(style.color.getCurrent());
  pen.setWidth(w);
  brush.setColor(style.color.getCurrent());
  painter->setBrush(brush);
  painter->setPen(pen);

  painter->setRenderHint(QPainter::Antialiasing, true);

  switch (style.pointStyle)
  {
  case zg::PlotStyle::Cross:
  {
    painter->drawLine(pt + QPointF(0, 2 * w), pt + QPointF(0, -2 * w));
    painter->drawLine(pt + QPointF(-2 * w, 0), pt + QPointF(2 * w, 0));
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
    w *= 2;
    QPolygonF polygon;
    double d = w * coef;
    double b = w / 2;

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
  const auto start = std::chrono::high_resolution_clock::now();

  for (const auto& [_, f_curve]: sampler.getCurves())
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

    currentSection.reserve(remainingNumPoints);
  };

  auto causes_flush = [&](size_t i) {
    const auto& pt = curve[i];
    return std::isnan(pt.x()) or std::isnan(pt.y()) or sampledCurve.discontinuities.contains(i);
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

    const QPointF P = curve[i] * scale;

    if (not sampledCurve.discrete and currentSection.size() >= 2 and i + 1 < curve.size() and not causes_flush(i+1))
    {
      const QPointF& A = currentSection[currentSection.size() - 2];
      const QPointF& B = currentSection.back();

      if (sq_dist_to_line(A, P, B) >= 0.125)
        currentSection.append(P);
    }
    else
      currentSection.append(P);
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
    pen.setWidth(style.lineWidth);
    pen.setColor(style.color.getCurrent());
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

  if (f_curve.discrete)
  {
    if (style.pointStyle != zg::PlotStyle::PointStyle::None)
      for (const QPointF& P: f_curve.px_curve)
      {
        if (not std::isnan(P.x()) and not std::isnan(P.y()))
          drawDataPoint(P, style);
      }
  }
}
