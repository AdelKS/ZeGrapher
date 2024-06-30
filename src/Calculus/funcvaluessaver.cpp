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
#include "Utils/algos.h"

FuncValuesSaver::FuncValuesSaver(const zg::ZeViewMapper& mapper, double pxStep)
  : mapper(mapper), pixelStep(zg::pixel_unit{pxStep})
{}

void FuncValuesSaver::setPixelStep(double pxStep)
{
  pixelStep = zg::pixel_unit{pxStep};
}

void FuncValuesSaver::clear_hidden_pts()
{
  const zg::view_unit viewUnitStep = pixelStep / mapper.x.getRange<zg::pixel>().amplitude()
                                     * mapper.x.getRange<zg::view>().amplitude();

  zg::Range1D<zg::u<zg::view>> range{.min = mapper.x.getRange<zg::view>().min - viewUnitStep,
                                     .max = mapper.x.getRange<zg::view>().max + viewUnitStep};


  if (not bool(viewRange.intersection(range)))
  {
    for (FuncCurve& funcCurve: funCurves)
      funcCurve.curve.clear();

    return;
  }

  for (FuncCurve& funcCurve: funCurves)
  {
    // clear from the right
    size_t vals_to_pop = 0;
    while (not funcCurve.curve.empty()
           and vals_to_pop < funcCurve.curve.size()
           and mapper.x.to<zg::view>((funcCurve.curve.crbegin() + vals_to_pop)->x)
                 >= range.max)
      vals_to_pop++;

    funcCurve.curve.resize(funcCurve.curve.size() - vals_to_pop);

    // clear from the left
    vals_to_pop = 0;
    while (not funcCurve.curve.empty()
           and vals_to_pop < funcCurve.curve.size()
           and mapper.x.to<zg::view>(funcCurve.curve[vals_to_pop].x) <= range.min)

      vals_to_pop++;

    funcCurve.curve.erase(funcCurve.curve.begin(), funcCurve.curve.begin() + vals_to_pop);
  }
}

void FuncValuesSaver::refresh_valid_functions()
{
  for (const FuncCurve& f_curve: funCurves)
  {
    const size_t slot = f_curve.slot;
    if (not information.getMathWorld().get<zc::Function<zc_t>>(slot))
    {
      qInfo() << "clearing slot " << slot << "from cached functions";
      funCurves.free(slot);
    }
  }

  for (const zc::DynMathObject<zc_t> &math_obj : information.getMathWorld())
  {
    if (not math_obj.holds<zc::Function<zc_t>>() or funCurves.is_assigned(math_obj.get_slot()))
      continue;

    qInfo() << "caching new function " << math_obj.get_name() << " slot " << math_obj.get_slot();

    funCurves.push(
      FuncCurve{
        .func = math_obj.value_as<zc::Function<zc_t>>(),
        .slot = math_obj.get_slot(),
        .equation = math_obj.value_as<zc::Function<zc_t>>().get_equation(),
      },
      math_obj.get_slot());
  }
}

void FuncValuesSaver::compute_uniform_visible_pts(size_t slot)
{
  Q_ASSERT(funCurves.is_assigned(slot));

  const zg::view_unit viewUnitStep = pixelStep / mapper.x.getRange<zg::pixel>().amplitude()
                                     * mapper.x.getRange<zg::view>().amplitude();

  using RangeT = zg::Range1D<zg::u<zg::view>>;
  RangeT current_range{.min = mapper.x.getRange<zg::view>().min - viewUnitStep,
                       .max = mapper.x.getRange<zg::view>().max + viewUnitStep};

  auto& f_curve = funCurves[slot];

  auto get_func_y = [&f = f_curve.func](zg::real_unit x_real)
  {
    tl::expected<double, zc::Error> exp_y_real = f(x_real.v);

    if (exp_y_real.has_value())
      return zg::real_unit{*exp_y_real};
    else return zg::real_unit{std::nan("")};
  };

  auto add_pts = [&]<Side side>(std::integral_constant<Side, side>)
  {
    RangeT loop_range;
    if (f_curve.curve.empty()) [[unlikely]]
      loop_range = current_range;
    else
    {
      if constexpr (side == LEFT)
      {
        loop_range.min = current_range.min;
        loop_range.max = mapper.x.to<zg::view>(f_curve.curve.front().x);
      }
      else
      {
        loop_range.min = mapper.x.to<zg::view>(f_curve.curve.back().x) + viewUnitStep;
        loop_range.max = current_range.max;
      }
    }

    std::vector<zg::real_pt> extra_pts;
    for (zg::view_unit x_view = loop_range.min; x_view < loop_range.max; x_view += viewUnitStep)
    {
      zg::real_unit x_real = mapper.x.to<zg::real>(x_view);
      extra_pts.push_back({.x = x_real, .y = get_func_y(x_real)});
    }

    if (not extra_pts.empty())
    {
      if constexpr (side == LEFT)
      {
        qInfo() << "function slot " << slot << " - added " << extra_pts.size() << " uniform pts LEFT";
        zg::utils::move_elements_right(f_curve.curve, extra_pts.size());
        std::ranges::copy(extra_pts, f_curve.curve.begin());
      }
      else
      {
        qInfo() << "function slot " << slot << " - added " << extra_pts.size() << " uniform pts RIGHT";
        f_curve.curve.reserve(f_curve.curve.size() + extra_pts.size());
        std::ranges::copy(extra_pts, std::back_inserter(f_curve.curve));
      }
    }
  };

  add_pts(std::integral_constant<Side, LEFT>());
  add_pts(std::integral_constant<Side, RIGHT>());
}

void FuncValuesSaver::refine_visible_pts(size_t slot)
{
  const zg::pixel_unit minPixelStep = pixelStep / double(pxStepMaxDivider);
  const double pxStepSq = pixelStep.v * pixelStep.v;

  Q_ASSERT(funCurves.is_assigned(slot));

  auto& f_curve = funCurves[slot];
  auto& curve = f_curve.curve;

  struct ExtraPt
  {
    size_t i;
    zg::real_pt pt;
  };

  auto get_func_y = [&f = f_curve.func](zg::real_unit x_real)
  {
    tl::expected<double, zc::Error> exp_y_real = f(x_real.v);

    if (exp_y_real.has_value())
      return zg::real_unit{*exp_y_real};
    else return zg::real_unit{std::nan("")};
  };

  std::vector<ExtraPt> extra_pts;
  do {
    curve.reserve(curve.size() + extra_pts.size());
    if (not extra_pts.empty())
    {
      qInfo() << "function slot " << slot << " added " << extra_pts.size()
              << " extra points in refining stage.";
      size_t inserted_num = 0;
      for (const ExtraPt& xpt: extra_pts)
      {
        curve.insert(curve.begin() + xpt.i + inserted_num, xpt.pt);
        inserted_num++;
      }
    }

    extra_pts.clear();

    for (size_t i = 0 ; i+2 != curve.size() ; i++)
    {
      if (std::isnan(curve[i].y.v) or std::isnan(curve[i+1].y.v))
        continue;

      const zg::pixel_pt px_pt0 = mapper.to<zg::pixel>(curve[i]);
      const zg::pixel_pt px_pt1 = mapper.to<zg::pixel>(curve[i+1]);

      if ((px_pt0 - px_pt1).square_length() > pxStepSq and (px_pt0.x - px_pt1.x).length() > minPixelStep)
      {
        // the two points are too far apart, and we can still subdivide
        zg::real_unit real_x = mapper.x.to<zg::real>(0.5 * px_pt0.x + 0.5 * px_pt1.x);
        extra_pts.push_back({.i = i+1, .pt = {.x = real_x, .y = get_func_y(real_x)}});
      }
    }
  }
  while (not extra_pts.empty());
}

void FuncValuesSaver::find_discontinuities(size_t slot)
{
  const zg::pixel_unit minPixelStep = pixelStep / double(pxStepMaxDivider);
  const double pxStepSq = pixelStep.v * pixelStep.v;

  auto& f_curve = funCurves[slot];
  auto& curve = f_curve.curve;

  f_curve.discontinuities.clear();

  constexpr size_t slice_size = 5;
  if (curve.size() <= slice_size)
    return;

  std::vector<zg::pixel_pt> px_pts;
  px_pts.reserve(curve.size());

  std::ranges::transform(curve, std::back_inserter(px_pts),
                         [&](const zg::real_pt& pt) { return mapper.to<zg::pixel>(pt); });

  std::vector<double> px_sq_lengths;
  px_sq_lengths.reserve(curve.size());

  for (size_t j = 0 ; j+1 != curve.size() ; j++)
    px_sq_lengths.push_back((px_pts[j+1] - px_pts[j]).square_length());

  std::array<zg::real_pt, slice_size> sub_arr;
  std::array<zg::pixel_pt, slice_size> sub_px_arr;
  std::array<double, slice_size-1> sub_sql_arr;

  for (size_t i = 0 ; i+slice_size != curve.size() ; i++)
  {
    auto subrange = std::ranges::subrange(curve.begin() + i, curve.begin() + i + slice_size);
    auto px_subrange = std::ranges::subrange(px_pts.begin() + i, px_pts.begin() + i + slice_size);
    auto px_sql_subrange = std::ranges::subrange(px_sq_lengths.begin() + i, px_sq_lengths.begin() + i + slice_size - 1);

    std::ranges::copy(subrange, sub_arr.begin());
    std::ranges::copy(px_subrange, sub_px_arr.begin());
    std::ranges::copy(px_sql_subrange, sub_sql_arr.begin());

    if (std::ranges::any_of(subrange, [](const zg::real_pt& pt){ return std::isnan(pt.y.v); }))
      continue;

    if (std::ranges::all_of(px_sql_subrange, [pxStepSq](double sql){ return sql > pxStepSq; }))
    {
      constexpr auto indices = std::views::iota(0ul, slice_size-2);
      if (std::ranges::all_of(indices, [&](size_t j) { return subrange[j].y <= subrange[j + 1].y; }))
        if(subrange.front().y >= subrange.back().y)
          f_curve.discontinuities.insert(i+slice_size-1);
      if (std::ranges::all_of(indices, [&](size_t j) { return subrange[j].y >= subrange[j + 1].y; }))
        if(subrange.front().y <= subrange.back().y)
          f_curve.discontinuities.insert(i+slice_size-1);
    }

    if (f_curve.discontinuities.contains(i+slice_size/2+1))
      continue;

    if (px_sql_subrange[slice_size/2] > pxStepSq)
    {
      Q_ASSERT((px_subrange[slice_size/2].x - px_subrange[slice_size/2+1].x).length() <= minPixelStep);
      // the step above should've already subdivided here, so we should be under minViewUnitStep

      if (px_sql_subrange[slice_size / 2] >= 4 * px_sql_subrange[slice_size / 2 - 1]
          and px_sql_subrange[slice_size / 2] >= 4 * px_sql_subrange[slice_size / 2 + 1])
        // no explanation, this is just an intuition of mine haha
        f_curve.discontinuities.insert(i+slice_size/2+1);
    }
  }
  qInfo() << "function slot " << slot << " found : " << f_curve.discontinuities.size() << " discontinuities";
}

void FuncValuesSaver::check_for_equation_changes()
{
  // should come after refresh_valid_functions()
  for (FuncCurve& f_curve: funCurves)
  {
    if (f_curve.equation != f_curve.func.get_equation())
    {
      f_curve.curve.clear();
      f_curve.discontinuities.clear();
      f_curve.equation = f_curve.func.get_equation();
    }
  }
}

void FuncValuesSaver::update()
{
  refresh_valid_functions();
  check_for_equation_changes();
  clear_hidden_pts();

  // TODO: this can be multi-threaded
  //       issue: simultaneous plotting according to a global constant to be thought through
  for (const auto& f_curve: funCurves)
  {
    compute_uniform_visible_pts(f_curve.slot);
    refine_visible_pts(f_curve.slot);
    find_discontinuities(f_curve.slot);
  }

}
