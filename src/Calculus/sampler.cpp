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

Sampler::Sampler(const zg::ZeViewMapper& mapper, double pxStep)
  : mapper(mapper), pixelStep(zg::pixel_unit{pxStep})
{}

Sampler::SamplingSettings::SamplingSettings(const zg::MathObject& obj, const zg::PlotStyle& style)
{
  step = style.getStep();
  coordinateSystem = style.coordinateSystem;
  revision = obj.getRevision();
}

void Sampler::setPixelStep(double pxStep)
{
  pixelStep = zg::pixel_unit{pxStep};
}

void Sampler::refresh_valid_objects()
{
  std::unordered_map<const zg::MathObject*, zg::SampledCurve> refreshed_curves;

  [[maybe_unused]] const auto& objs = zg::mathWorld.getMathObjects();
  for (auto&& [f, style] : zg::mathWorld.getMathObjects())
  {
    auto sampled_settings_node = sampled_settings.extract(f);
    auto curve_node = curves.extract(f);

    if (not style or not f->isValid()
        or std::isnan(style->getStart().v)
        or std::isnan(style->getEnd().v)
        or (f->isDiscrete() and std::isnan(style->getStep().v))
        or style->getStart() >= style->getEnd()
        or f->getType() == zg::MathObject::CONSTANT
        or f->getType() == zg::MathObject::EXPR
        )
      continue;

    const auto sampling_settings = SamplingSettings(*f, *style);

    assert((sampled_settings_node and curve_node)
            or (not sampled_settings_node and not curve_node));

    if (curve_node
        and sampled_settings_node.mapped() == sampling_settings)
      refreshed_curves.insert(std::move(curve_node));
    else
    {
      zg::SampledCurve curve(*style);
      curve.discrete = f->isDiscrete();
      refreshed_curves.emplace(f, std::move(curve));
    }

    sampled_settings.emplace(f, std::move(sampling_settings));
  }

  curves = std::move(refreshed_curves);
}

void Sampler::update()
{
  refresh_valid_objects();

  auto dispatch = [this](zg::MathObject::EvalHandle var_handle, auto& data)
  {
    std::visit(zc::utils::overloaded{
      [](std::monostate){},
      [&](auto handle){
        if (data.style.coordinateSystem == zg::PlotStyle::Cartesian)
          sample<zg::PlotStyle::Cartesian>(handle, data);
        else if (data.style.coordinateSystem == zg::PlotStyle::Polar)
          sample<zg::PlotStyle::Polar>(handle, data);
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

  const auto end = std::chrono::high_resolution_clock::now();

  qDebug() << "Sampling took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

}

void Sampler::update_discontinuities(zg::SampledCurve& data)
{
  data.discontinuities.clear();

  const auto& input_vals = data.input;
  const auto& curve = data.curve;

  const zg::real_unit min_step = data.get_smallest_allowed_step();
  const zg::real_unit max_step = data.get_biggest_allowed_step();
  const double sq_px_step = pixelStep.v * pixelStep.v;

  auto is_nan_pt = [](const zg::real_pt& pt) {
    return std::isnan(pt.x.v) or std::isnan(pt.y.v);
  };

  size_t label = 1;
  qDebug() << "Object caching: curve has " << curve.size() << " points";

  for (size_t i = 0; i + 1 < input_vals.size(); i++)
  {
    const zg::real_pt& B = curve[i];
    const QPointF& px_B = data.px_curve[i];

    const zg::real_pt& C = curve[i+1];
    const QPointF& px_C = data.px_curve[i+1];

    const zg::real_unit bc = input_vals[i+1] - input_vals[i];
    const QPointF px_BC = px_C - px_B;
    const double px_sq_BC = QPointF::dotProduct(px_BC, px_BC);

    if (is_nan_pt(B) or is_nan_pt(C))
      continue;

    if (bc >= 2 * min_step or (bc <= max_step and px_sq_BC <= sq_px_step))
      continue;

    bool is_discontinuity = true;
    if (i > 0)
    {
      const QPointF& px_A = data.px_curve[i-1];
      const QPointF px_AB = px_B - px_A;
      const double dot = QPointF::dotProduct(px_AB, px_BC);
      if (sq_dist_to_segment(px_A, px_B, px_C) < 4 * sq_px_step and dot > 0
          and dot * dot > 0.9 * QPointF::dotProduct(px_AB, px_AB) * px_sq_BC)
        is_discontinuity = false;
    }
    if (is_discontinuity and i + 2 < input_vals.size())
    {
      const QPointF& px_D = data.px_curve[i+2];
      const QPointF px_CD = px_D - px_C;
      const double dot = QPointF::dotProduct(px_BC, px_CD);
      if (sq_dist_to_segment(px_B, px_C, px_D) < 4 * sq_px_step and dot > 0
          and dot * dot > 0.9 * px_sq_BC * QPointF::dotProduct(px_CD, px_CD))
        is_discontinuity = false;
    }

    if (is_discontinuity)
    {
      data.discontinuities.insert(i+1);
      qDebug() << "- Discontinuity " << label++ << ": between ( " << B.x.v << ", " << B.y.v
               << " ) and (" << C.x.v << ", " << C.y.v << " )";
    }
  }

  qDebug() << "  and has " << data.discontinuities.size() << " discontinuities.";
}

void Sampler::clearCache(QStringList objectNames)
{
  refresh_valid_objects();

  for (auto& [f, curve]: curves)
    if (objectNames.contains(f->getName()))
      curve.clear();
}
