/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include "Calculus/funcvaluessaver.h"
#include "information.h"

FuncValuesSaver::FuncValuesSaver(const zg::ZeViewMapper& mapper, double pxStep)
  : mapper(mapper), pixelStep(zg::pixel_unit{pxStep})
{}

void FuncValuesSaver::setPixelStep(double pxStep)
{
  pixelStep = zg::pixel_unit{pxStep};
}

void FuncValuesSaver::calculateAll()
{
  const zg::view_unit viewUnitStep = pixelStep / mapper.x.getRange<zg::pixel>().amplitude()
                               * mapper.x.getRange<zg::view>().amplitude();

  funCurves.clear();

  const zg::view_unit xViewStart = mapper.x.getRange<zg::view>().min - viewUnitStep;
  const zg::view_unit xViewEnd = mapper.x.getRange<zg::view>().max + viewUnitStep;

  auto is_normal_double = [](double x){ return not std::isnan(x) and not std::isinf(x); };

  const double pxStepSq = pixelStep.v * pixelStep.v;

  for (const zc::DynMathObject<zc_t> &math_obj : information.getMathWorld())
  {
    if (not math_obj.holds<zc::Function<zc_t>>())
      continue;

    const zc::Function<zc_t>& func = math_obj.value_as<zc::Function<zc_t>>();

    FuncCurve funcCurve {.func = func};

    zg::view_unit xView = xViewStart;
    std::vector<zg::real_pt> slice;
    size_t subdiv = 0;
    constexpr size_t max_subdiv = 4;
    zg::view_unit currentViewUnitStep = viewUnitStep;
    while (xView <= xViewEnd)
    {
      zg::real_unit x = mapper.x.to<zg::real>(xView);
      if (x.v > 1.)
        qDebug() << "Here!";
      tl::expected<double, zc::Error> res = func(x.v);
      if (not (res and is_normal_double(*res) and mapper.y.isInView(zg::real_unit{*res}))) [[unlikely]]
      {
        if (not slice.empty())
          funcCurve.slices.push_back(std::move(slice));

        subdiv = 0;
        xView += currentViewUnitStep;
        continue;
      }

      auto new_pt = zg::real_pt{.x = x, .y = zg::real_unit{*res}};
      if (slice.empty())
      {
        subdiv = 0;
        slice.push_back(new_pt);
        xView += currentViewUnitStep;
      }
      else
      {
        auto diff = mapper.to<zg::pixel>(new_pt) - mapper.to<zg::pixel>(slice.back());
        const double sqDist = diff.x.v * diff.x.v + diff.y.v * diff.y.v;
        if (sqDist > pxStepSq)
        {
          // we are not under requested curve quality
          if (subdiv < max_subdiv)
          {
            // subdivide and see if it's good after that
            currentViewUnitStep /= 2.;
            xView = 0.5 * xView + 0.5 * mapper.x.to<zg::view>(slice.back().x);
            subdiv++;
          }
          else
          {
            // we attained max subdiv amount, likely a discontinuity
            // save the function and continue
            subdiv = 0;
            funcCurve.slices.push_back(std::move(slice));
            xView += currentViewUnitStep;
          }
        }
        else
        {
          subdiv = 0;
          if (4 * sqDist < pxStepSq)
            // step a bit too small
            currentViewUnitStep *= 2.;

          slice.push_back(new_pt);
          xView += currentViewUnitStep;
        }
      }
    }

    if (not slice.empty())
      funcCurve.slices.push_back(std::move(slice));

    if (not funcCurve.slices.empty())
      funCurves.push_back(std::move(funcCurve));
  }
}

void FuncValuesSaver::move()
{
  calculateAll();
}
