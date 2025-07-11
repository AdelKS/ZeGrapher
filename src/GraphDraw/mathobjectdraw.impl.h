#pragma once

#include "information.h"
#include "mathobjectdraw.h"

template <zg::CurveType t>
void MathObjectDraw::drawSampledCurve(const zg::SampledCurve<t>& f_curve)
{
  if (not f_curve.style.visible)
    return;

  painter->setRenderHint(QPainter::Antialiasing,
                         information.getGraphSettings().smoothing && !moving);

  pen.setColor(Qt::red);
  painter->setPen(pen);

  const auto& style = f_curve.style;

  std::vector<QPointF> mapped_curve;
  auto draw_mapped_curve = [&]()
  {
    if (not mapped_curve.empty() and style.lineStyle != Qt::PenStyle::NoPen)
    {
      pen.setWidth(style.lineWidth);
      pen.setColor(style.color);
      pen.setStyle(style.lineStyle);
      painter->setPen(pen);
      painter->drawPolyline(mapped_curve.data(), mapped_curve.size());
      mapped_curve.clear();
    }
  };

  const auto& curve = f_curve.get_curve();

  for (size_t i = 0; i != curve.size(); i++)
  {
    const auto& pt = curve[i];

    if (std::isnan(pt.y.v) or std::isnan(pt.x.v))
    {
      draw_mapped_curve();
    }
    else
    {
      if constexpr (t == zg::CurveType::CONTINUOUS)
      {
        if (f_curve.discontinuities.contains(i))
          draw_mapped_curve();
      }

      auto px_pt = QPointF(viewMapper.to<zg::pixel>(pt));
      if (style.lineStyle != Qt::PenStyle::NoPen)
        mapped_curve.push_back(px_pt);

      if constexpr (t == zg::CurveType::DISCRETE)
        drawDataPoint(px_pt, style);
    }
  }

  draw_mapped_curve();

}
