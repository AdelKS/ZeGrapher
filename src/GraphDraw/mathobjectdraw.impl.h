#pragma once

#include "Utils/sampledcurve.impl.h"
#include "globalvars.h"
#include "mathobjectdraw.h"
#include "viewmapper.h"

/// @brief reduces the amount of points of sampledCurve and splits on discontinuities and NaN points
inline QList<QPolygonF> buildFinalCurve(const zg::SampledCurve<zg::CurveType::CONTINUOUS>& sampledCurve,
                                 double scale = 1.0)
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

    if (currentSection.size() >= 2 and i + 1 < curve.size() and not causes_flush(i+1))
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


template <zg::CurveType t>
void MathObjectDraw::drawSampledCurve(const zg::SampledCurve<t>& f_curve)
{
  if (not f_curve.style.visible)
    return;

  painter->setRenderHint(QPainter::Antialiasing);

  const auto& style = f_curve.style;

  if (style.lineStyle != Qt::NoPen)
  {
    pen.setWidth(style.lineWidth);
    pen.setColor(style.color.getCurrent());
    pen.setStyle(style.lineStyle);
    painter->setPen(pen);

    if constexpr (t == zg::CurveType::DISCRETE)
      painter->drawPolyline(f_curve.px_curve.data(), f_curve.px_curve.size());
    else
    {
      const QList<QPolygonF> curve = buildFinalCurve(f_curve);

      size_t plotted_points = 0;
      for (const QPolygonF& seg : curve)
      {
        painter->drawPolyline(seg);
        plotted_points += seg.size();
      }

      qDebug() << "Plotted continuous curve with" << plotted_points << "points.";
    }
  }

  if constexpr (t == zg::CurveType::DISCRETE)
  {
    if (style.pointStyle == zg::PlotStyle::PointStyle::None)
      return;

    for (const QPointF& P: f_curve.px_curve)
    {
      if (not std::isnan(P.x()) and not std::isnan(P.y()))
        drawDataPoint(P, style);
    }
  }
}
