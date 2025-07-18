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
#include "Utils/sampledcurve.impl.h"
#include "information.h"
#include "sampler.impl.h"

Sampler::Sampler(const zg::ZeViewMapper& mapper, double pxStep)
  : mapper(mapper), pixelStep(zg::pixel_unit{pxStep})
{}

void Sampler::setPixelStep(double pxStep)
{
  pixelStep = zg::pixel_unit{pxStep};
}

void Sampler::refresh_valid_objects()
{
  std::unordered_map<const zg::MathObject*, zg::SampledCurveContinuous> refreshed_continuous_curves;
  std::unordered_map<const zg::MathObject*, zg::SampledCurveDiscrete> refreshed_discrete_curves;

  for (const zg::MathObject* f : information.getMathObjects())
  {
    if (not f->style or f->getState().getStatus() != zg::State::VALID)
      continue;

    auto sampled_settings_node = sampled_settings.extract(f);
    auto discrete_curve_node = discrete_curves.extract(f);
    auto continuous_curve_node = continuous_curves.extract(f);

    if (f->style->objectType == zg::PlotStyle::Continuous)
    {
      assert((sampled_settings_node and continuous_curve_node)
             or (not sampled_settings_node and not continuous_curve_node));

      if (continuous_curve_node
          and sampled_settings_node.mapped() == f->style->get_sampling_settings())
        refreshed_continuous_curves.insert(std::move(continuous_curve_node));
      else
        refreshed_continuous_curves.emplace(f, zg::SampledCurveContinuous(*f->style));

      sampled_settings.emplace(f, f->style->get_sampling_settings());
    }
    else if (f->style->objectType == zg::PlotStyle::Discrete)
    {
      assert((sampled_settings_node and discrete_curve_node)
             or (not sampled_settings_node and not discrete_curve_node));

      if (discrete_curve_node
          and sampled_settings_node.mapped() == f->style->get_sampling_settings())
        refreshed_discrete_curves.insert(std::move(discrete_curve_node));
      else
        refreshed_discrete_curves.emplace(f, zg::SampledCurveDiscrete(*f->style));

      sampled_settings.emplace(f, f->style->get_sampling_settings());
    }
  }

  continuous_curves = std::move(refreshed_continuous_curves);
  discrete_curves = std::move(refreshed_discrete_curves);
}

void Sampler::update()
{
  refresh_valid_objects();

  // TODO: this can be multi-threaded
  //       issue: simultaneous plotting according to a global constant to be thought through
  for (auto& [f, data]: continuous_curves)
    sample(*f, data);

  for (auto& [f, data]: discrete_curves)
    sample(*f, data);

}

void Sampler::clearCache(QStringList objectNames)
{
  refresh_valid_objects();

  for (auto& [f, curve]: continuous_curves)
    if (objectNames.contains(f->getName()))
      curve.clear();
}
