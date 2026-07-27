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
#include "information.h"
#include "sampler.impl.h"

#include <cmath>

Sampler::Sampler(const zg::ZeViewMapper& mapper)
  : mapper(mapper)
{}

static zg::CurveStyle make_curve_style(const zg::PlotStyle& ps)
{
  return zg::CurveStyle{
    .visible = ps.visible,
    .color = ps.color.getCurrent(),
    .lineWidth = ps.lineWidth,
    .dashPattern = ps.getDashPattern(),
    .drawLine = ps.lineStyle != zg::PlotStyle::LineStyle::NoLine,
    .pointWidth = ps.pointWidth,
    .pointStyle = static_cast<zg::CurveStyle::PointStyle>(ps.pointStyle),
  };
}

/// @brief the objects to sample, in the order they are listed in
static std::vector<Sampler::Plottable> plottables()
{
  std::vector<Sampler::Plottable> objects;

  for (zg::MathObject* o: zg::mathWorld.getMathObjects())
  {
    if (zg::mathobj::DataSheet* dataSheet = o->getDataSheet())
      for (zg::mathobj::Data* column: dataSheet->getColumns())
        objects.push_back(column);
    else objects.push_back(o);
  }

  return objects;
}

void Sampler::refresh_schrodinger_keys()
{
  schrodinger_constant = zg::mathWorld.getSchrodingerConstant();
  if (not schrodinger_constant)
  {
    schrodinger_curves_map.clear();
    return;
  }

  assert(schrodinger_constant->getSteps() > 0);

  if (std::isnan(schrodinger_constant->getFrom()) or std::isnan(schrodinger_constant->getTo()))
    return;

  std::unordered_set<double> current_values;
  for (int i = 0 ; i <= schrodinger_constant->getSteps(); i++)
  {
    double t = double(i) / double(schrodinger_constant->getSteps());
    current_values.insert(
      std::lerp(schrodinger_constant->getFrom(), schrodinger_constant->getTo(), t));
  }

  std::erase_if(schrodinger_curves_map,
                [&](const auto& item) { return not current_values.contains(item.first); });

  for (double v: current_values)
    schrodinger_curves_map.try_emplace(v);
}

void Sampler::refresh_valid_objects()
{
  auto sampling_settings = [](auto* f) {
    return f->getSamplingSettings();
  };

  auto is_plottable = []<typename T>(T* f) {
    if constexpr (std::is_same_v<T, zg::MathObject>)
      return f->isValid() and f->getType() != zg::MathObject::CONSTANT;
    else if constexpr (std::is_same_v<T, zg::mathobj::Data>)
      return f->isValid();
    else static_assert(zc::utils::dependent_false_v<T>, "Case not covered");
  };

  auto update = [&](auto* f, CurveMap& c, bool good)
  {
    const auto opt_sampling_settings = sampling_settings(f);
    if (good && bool(opt_sampling_settings))
    {
      auto& new_settings = *opt_sampling_settings;
      auto& curve = c[f];
      curve.discrete = f->isDiscrete();
      if (curve.settings.coordinateSystem != new_settings.coordinateSystem or
          curve.settings.revision != new_settings.revision)
        curve.clear();
      curve.settings = new_settings;
    }
    else
      c.erase(f);
  };

  const std::vector<Plottable> objects = plottables();

  for (Plottable p: objects)
    std::visit([&](auto* f) { update(f, curves, is_plottable(f) and not f->isSchrodinger()); }, p);

  for (auto& [v, schrodinger_curves]: schrodinger_curves_map)
  {
    schrodinger_constant->zcMathObj = v;
    for (Plottable p: objects)
      std::visit([&](auto* f) { update(f, schrodinger_curves, is_plottable(f) and f->isSchrodinger()); }, p);
  }

  auto clearDeletedObjects = [&](CurveMap& c) {
    auto it = c.begin();
    while(it != c.end())
      if (std::ranges::find(objects, it->first) == objects.end())
        it = c.erase(it);
      else it++;
  };

  clearDeletedObjects(curves);
  for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
    clearDeletedObjects(schrodinger_curves);
}

void Sampler::refresh_curve_styles()
{
  for (Plottable p : plottables())
    std::visit([this](auto* f) {
      if (auto it = curves.find(f); it != curves.end())
        it->second.style = make_curve_style(*f->getStyle());

      for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
        if (auto it = schrodinger_curves.find(f); it != schrodinger_curves.end())
          it->second.style = make_curve_style(*f->getStyle());
    }, p);
}

void Sampler::refresh_curves_list()
{
  curves_list.clear();

  for (Plottable p : std::views::reverse(plottables()))
    std::visit([this](auto* f) {
      if (auto it = curves.find(f); it != curves.end())
        curves_list.push_back(it->second);

      for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
        if (auto it = schrodinger_curves.find(f); it != schrodinger_curves.end())
          curves_list.push_back(it->second);
    }, p);
}

void Sampler::update()
{
  refresh_schrodinger_keys();
  refresh_valid_objects();
  refresh_curve_styles();
  refresh_curves_list();

  auto dispatch = [this](zg::EvalHandle var_handle, auto& data)
  {
    std::visit(zc::utils::overloaded{
      [&](auto handle){
        if (data.settings.coordinateSystem == zg::CoordinateSystem::Cartesian)
        {
          if (data.discrete)
            sample<zg::CoordinateSystem::Cartesian, true>(handle, data);
          else sample<zg::CoordinateSystem::Cartesian, false>(handle, data);
        }
        else if (data.settings.coordinateSystem == zg::CoordinateSystem::Polar)
        {
          if (data.discrete)
            sample<zg::CoordinateSystem::Polar, true>(handle, data);
          else sample<zg::CoordinateSystem::Polar, false>(handle, data);
        }
        else qCritical() << "Case not handled, aborting program";
      }
    }, var_handle);
  };

  // TODO: this can be multi-threaded
  //       issue: simultaneous plotting according to a global constant to be thought through

  const auto start = std::chrono::high_resolution_clock::now();

  auto eval_handle = [](auto* f) {
    return f->getZcObject();
  };

  for (auto& [p, data]: curves)
  {
    if (not data.style.visible)
      continue;

    std::visit([&](auto* f) {
      dispatch(eval_handle(f), data);
      if (f->isContinuous())
        update_discontinuities(data);
    }, p);
  }

  if (schrodinger_constant)
  {
    for (auto& [v, schrodinger_curves]: schrodinger_curves_map)
    {
      schrodinger_constant->zcMathObj = v;
      double amplitude = schrodinger_constant->getTo() - schrodinger_constant->getFrom();
      double t = amplitude != 0 ? (v - schrodinger_constant->getFrom()) / amplitude : 1.0;
      for (auto& [p, data]: schrodinger_curves)
        std::visit([&](auto* f) {
          data.style.color = f->getStyle()->colorLerp(t);
          dispatch(eval_handle(f), data);
          if (f->isContinuous())
            update_discontinuities(data);
        }, p);
    }
    schrodinger_constant->zcMathObj = std::nan("");
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
    const zg::real_pt& A = curve[i];
    const QPointF& px_A = data.px_curve[i];

    const zg::real_pt& B = curve[i+1];
    const QPointF& px_B = data.px_curve[i+1];

    const zg::real_pt& C = curve[i+2];
    const QPointF& px_C = data.px_curve[i+2];

    if (is_nan_pt(A) or is_nan_pt(B) or is_nan_pt(C))
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

  auto searchAndClear = [&objectNames](CurveMap& c) {
    for (auto& [p, curve]: c)
      if (std::visit([&](auto* f) { return objectNames.contains(f->getName()); }, p))
        curve.clear();
  };

  searchAndClear(curves);
  for (auto& [_, schrodinger_curves]: schrodinger_curves_map)
    searchAndClear(schrodinger_curves);
}
