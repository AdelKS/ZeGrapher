/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "Calculus/sampler.h"
#include "globalvars.h"
#include "sampler.impl.h"

#include <cmath>

Sampler::Sampler(const zg::ZeViewMapper& mapper, double pxStep)
  : mapper(mapper), pixelStep(zg::pixel_unit{pxStep})
{}

void Sampler::setPixelStep(double pxStep)
{
  pixelStep = zg::pixel_unit{pxStep};
}

static zg::CurveStyle make_curve_style(const zg::PlotStyle& ps)
{
  return zg::CurveStyle{
    .visible = ps.visible,
    .color = ps.color.getCurrent(),
    .lineWidth = ps.lineWidth,
    .dashPattern = ps.dashPattern,
    .drawLine = ps.drawLine,
    .pointWidth = ps.pointWidth,
    .pointStyle = static_cast<zg::CurveStyle::PointStyle>(ps.pointStyle),
  };
}

void Sampler::refresh_schrodinger_keys()
{
  if (not zg::animationConductor.getSchrodingerConstant())
  {
    schrodinger_curves_map.clear();
    return;
  }

  const zg::AnimatedConstant* c = zg::animationConductor.getSchrodingerConstant();
  assert(c);
  assert(c->getSteps() > 0);

  if (std::isnan(c->getFrom()) or std::isnan(c->getTo()))
    return;

  std::unordered_set<double> current_values;
  for (size_t i = 0 ; i <= c->getSteps(); i++)
  {
    double t = double(i) / double(c->getSteps());
    current_values.insert(std::lerp(c->getFrom(), c->getTo(), t));
  }

  std::erase_if(schrodinger_curves_map,
                [&](const auto& item) { return not current_values.contains(item.first); });

  for (double v: current_values)
    schrodinger_curves_map.try_emplace(v);
}

void Sampler::refresh_valid_objects()
{
  using MapT = std::unordered_map<const zg::MathObject*, zg::SampledCurve>;

  auto update = [](zg::MathObject* f, MapT& c, bool good)
  {
    if (good)
      c[f].discrete = f->isDiscrete();
    else
      c.erase(f);
  };

  const auto& mathObjects = zg::mathWorld.getMathObjects();
  for (auto* f: mathObjects)
  {
    zg::PlotStyle* style = zg::mathWorld.getStyles()[f];
    bool good = not (not style or not f->isValid()
        or std::isnan(style->getStart().v)
        or std::isnan(style->getEnd().v)
        or (f->isDiscrete() and std::isnan(style->getStep().v))
        or style->getStart() >= style->getEnd()
        or f->getType() == zg::MathObject::CONSTANT
        or f->getType() == zg::MathObject::EXPR
        );

    update(f, curves, good && !f->isSchrodinger());
    for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
      update(f, schrodinger_curves, good && f->isSchrodinger());
  }

  auto clearDeletedObjects = [&](MapT& c) {
    auto it = c.begin();
    while(it != c.end())
      if (auto f = std::ranges::find(mathObjects, it->first); f == mathObjects.end())
        it = c.erase(it);
      else it++;
  };

  clearDeletedObjects(curves);
  for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
    clearDeletedObjects(schrodinger_curves);
}

void Sampler::refresh_curve_styles()
{
  for (auto* f : zg::mathWorld.getMathObjects())
  {
    zg::PlotStyle* style = zg::mathWorld.getStyles()[f];
    if (not style)
      continue;

    if (auto it = curves.find(f); it != curves.end())
      it->second.style = make_curve_style(*style);

    for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
      if (auto it = schrodinger_curves.find(f); it != schrodinger_curves.end())
        it->second.style = make_curve_style(*style);
  }
}

void Sampler::refresh_curve_settings()
{
  for (auto* f : zg::mathWorld.getMathObjects())
  {
    zg::PlotStyle* style = zg::mathWorld.getStyles()[f];
    if (not style)
      continue;

    if (auto it = curves.find(f); it != curves.end())
      it->second.update_sampling_settings(*f, *style);

    for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
      if (auto it = schrodinger_curves.find(f); it != schrodinger_curves.end())
        it->second.update_sampling_settings(*f, *style);

  }
}

void Sampler::refresh_curves_list()
{
  curves_list.clear();

  for (auto* f : std::views::reverse(zg::mathWorld.getMathObjects()))
  {
    if (auto it = curves.find(f); it != curves.end())
      curves_list.push_back(it->second);

    for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
      if (auto it = schrodinger_curves.find(f); it != schrodinger_curves.end())
        curves_list.push_back(it->second);
  }
}

void Sampler::update()
{
  refresh_schrodinger_keys();
  refresh_valid_objects();
  refresh_curve_styles();
  refresh_curve_settings();
  refresh_curves_list();

  auto dispatch = [this](zg::MathObject::EvalHandle var_handle, auto& data)
  {
    std::visit(zc::utils::overloaded{
      [](std::monostate){},
      [&](auto handle){
        if (data.settings.coordinateSystem == zg::PlotStyle::Cartesian)
        {
          if (data.discrete)
            sample<zg::PlotStyle::Cartesian, true>(handle, data);
          else sample<zg::PlotStyle::Cartesian, false>(handle, data);
        }
        else if (data.settings.coordinateSystem == zg::PlotStyle::Polar)
        {
          if (data.discrete)
            sample<zg::PlotStyle::Polar, true>(handle, data);
          else sample<zg::PlotStyle::Polar, false>(handle, data);
        }
        else qCritical() << "Case not handled, aborting program";
      }
    }, var_handle);
  };

  // TODO: this can be multi-threaded
  //       issue: simultaneous plotting according to a global constant to be thought through

  const auto start = std::chrono::high_resolution_clock::now();

  for (auto& [f, data]: curves)
  {
    dispatch(f->getZcObject(), data);
    if (f->isContinuous())
      update_discontinuities(data);
  }

  if (zg::AnimatedConstant* c_obj = zg::animationConductor.getSchrodingerConstant())
  {
    zg::mathobj::Constant* c = c_obj->getBackend();
    assert(c);

    if (c) [[likely]]
    {
      for (auto& [v, schrodinger_curves]: schrodinger_curves_map)
      {
        c->zcMathObj = v;
        double amplitude = c_obj->getTo() - c_obj->getFrom();
        double t = amplitude != 0 ? (v - c_obj->getFrom()) / amplitude : 1.0;
        for (auto& [f, data]: schrodinger_curves)
        {
          zg::PlotStyle* style = zg::mathWorld.getStyles()[f];
          if (not style)
            continue;

          data.style.color = style->colorLerp(t);
          dispatch(f->getZcObject(), data);
          if (f->isContinuous())
            update_discontinuities(data);
        }
      }
      c->zcMathObj = std::nan("");
    }
  }

  const auto end = std::chrono::high_resolution_clock::now();

  qDebug() << "Sampling took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

}

void Sampler::update_discontinuities(zg::SampledCurve& data)
{
  data.discontinuities.clear();

  const auto& input_vals = data.input;
  const auto& curve = data.curve;

  auto is_nan_pt = [](const zg::real_pt& pt) {
    return std::isnan(pt.x.v) or std::isnan(pt.y.v);
  };

  size_t label = 1;
  qDebug() << "Object caching: curve has " << curve.size() << " points";

  for (size_t i = 0; i + 2 < input_vals.size(); i++)
  {
    const QPointF& px_A = data.px_curve[i];

    const zg::real_pt& B = curve[i+1];
    const QPointF& px_B = data.px_curve[i+1];

    const zg::real_pt& C = curve[i+2];
    const QPointF& px_C = data.px_curve[i+2];

    if (is_nan_pt(B) or is_nan_pt(C))
      continue;

    if (sq_dist_to_ray(px_A, px_B, px_C) < sq_dist_to_ray_limit)
      continue;

    data.discontinuities.insert(i+2);
    qDebug() << "- Discontinuity " << label++ << ": between ( " << B.x.v << ", " << B.y.v
              << " ) and (" << C.x.v << ", " << C.y.v << " )";

  }

  qDebug() << "  and has " << data.discontinuities.size() << " discontinuities.";
}

void Sampler::clearCache(QStringList objectNames)
{
  refresh_valid_objects();
  refresh_schrodinger_keys();

  for (auto& [f, curve]: curves)
    if (objectNames.contains(f->getName()))
      curve.clear();

  for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
  {
    for( auto& [f, curve]: schrodinger_curves)
      if (objectNames.contains(f->getName()))
        curve.clear();
  }
}
