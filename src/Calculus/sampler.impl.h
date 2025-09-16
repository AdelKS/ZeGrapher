#pragma once

#include "Utils/unit.h"
#include "information.h"
#include "sampler.h"

/// @brief returns the square distance between B and the segment [AB]
inline double sq_dist_to_segment(const zg::pixel_pt& A, const zg::pixel_pt& P, const zg::pixel_pt& B) {
  const zg::pixel_pt AB = B - A;
  const zg::pixel_pt AP = P - A;
  const double t = std::max(0., std::min(AP.dot(AB) / AB.square_length(), 1.));
  return (AP - t * AB).square_length();
};

template <zg::PlotStyle::CoordinateSystem coordinates, zg::CurveType t>
void Sampler::sample(auto handle, zg::SampledCurve<t>& data)
{
  auto get_acceptable_input = [&](zg::real_unit x)
  {
    if constexpr (t == zg::CurveType::DISCRETE)
      return std::round(std::max(0., x.v) / data.style.step.v) * data.style.step;
    else return x;
  };
  auto dispatcher = zc::utils::overloaded{
    [&](const zc::DynMathObject<zc_t>* f, zg::real_unit x)
      requires (coordinates == zg::PlotStyle::CoordinateSystem::Cartesian)
    {
      tl::expected<double, zc::Error> exp_res = (*f)({x.v}, &information.mathObjectCache);
      double res = exp_res ? *exp_res : std::nan("");
      return zg::real_pt{x, zg::real_unit{res}};
    },
    [&](const zc::DynMathObject<zc_t>* f, zg::real_unit x)
      requires (coordinates == zg::PlotStyle::CoordinateSystem::Polar)
    {
      tl::expected<double, zc::Error> exp_res = (*f)({x.v}, &information.mathObjectCache);
      double res = exp_res ? *exp_res : std::nan("");
      return zg::real_pt{std::cos(x.v) * zg::real_unit{res}, std::sin(x.v) * zg::real_unit{res}};
    },
    [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> f, zg::real_unit x)
      requires (coordinates == zg::PlotStyle::CoordinateSystem::Cartesian)
    {
      tl::expected<double, zc::Error> exp_res1 = (*(f.first))({x.v}, &information.mathObjectCache);
      double res1 = exp_res1 ? *exp_res1 : std::nan("");

      tl::expected<double, zc::Error> exp_res2 = (*(f.second))({x.v}, &information.mathObjectCache);
      double res2 = exp_res2 ? *exp_res2 : std::nan("");

      return zg::real_pt{zg::real_unit{res1}, zg::real_unit{res2}};
    },
    [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> f, zg::real_unit x)
      requires (coordinates == zg::PlotStyle::CoordinateSystem::Polar)
    {
      tl::expected<double, zc::Error> exp_res1 = (*(f.first))({x.v}, &information.mathObjectCache);
      double res1 = exp_res1 ? *exp_res1 : std::nan("");

      tl::expected<double, zc::Error> exp_res2 = (*(f.second))({x.v}, &information.mathObjectCache);
      double res2 = exp_res2 ? *exp_res2 : std::nan("");

      return zg::real_pt{std::cos(res1) * zg::real_unit{res2}, std::sin(res1) * zg::real_unit{res2}};
    }
  };

  auto get_f_pt = [&](zg::real_unit x)
  {
    return dispatcher(handle, x);
  };

  auto is_nan_pt = [](const zg::real_pt& pt)
  {
    return std::isnan(pt.x.v) or std::isnan(pt.y.v);
  };

  const double sq_px_step = t == zg::CurveType::CONTINUOUS
                              ? pixelStep.v * pixelStep.v
                              : data.style.pointWidth * data.style.pointWidth;

  const std::vector<zg::real_unit>& input_vals = data.get_input();
  const std::vector<zg::real_pt>& curve = data.get_curve();

  // clear from the right
  if (not input_vals.empty())
  {
    size_t vals_to_pop = 0;
    const auto max = get_acceptable_input(data.style.range.max);
    while (vals_to_pop < input_vals.size() and input_vals[input_vals.size() - vals_to_pop - 1] > max)
      vals_to_pop++;

    data.pop_back(vals_to_pop);

    vals_to_pop = 0;
    const auto min = get_acceptable_input(data.style.range.min);
    while (vals_to_pop < input_vals.size() and input_vals[vals_to_pop] < min)
      vals_to_pop++;

    data.pop_front(vals_to_pop);
  }

  std::string obj_name = zc::utils::overloaded{
    [&](const zc::DynMathObject<zc_t>* f)
    {
      return std::string(f->get_name());
    },
    [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> f)
    {
      return std::string(f.first->get_name()) + "," + std::string(f.second->get_name());
    },
  }(handle);

  qDebug() << "Object caching: " << obj_name
    << " sampling range: min=" << QString::number(data.style.range.min.v, 'g', 14)
    << " max=" << QString::number(data.style.range.max.v, 'g', 14);

  {
    const zg::real_unit smallest_allowed_step = data.get_smallest_allowed_step();
    const auto min = get_acceptable_input(data.style.range.min);
    const auto min_pt = get_f_pt(min);
    if (curve.empty()
        or (input_vals.front() - min >= smallest_allowed_step
            and (is_nan_pt(min_pt) or is_nan_pt(curve.front())
                 or (mapper.to<zg::pixel>(min_pt) - mapper.to<zg::pixel>(curve.front()))
                        .square_length()
                      > sq_px_step)))
      data.insert_pt(0, min, min_pt);
  }
  {
    const zg::real_unit smallest_allowed_step = data.get_smallest_allowed_step();
    const auto max = get_acceptable_input(data.style.range.max);
    const auto max_pt = get_f_pt(max);
    if (curve.size() == 1
        or (max - input_vals.front() >= smallest_allowed_step
            and (is_nan_pt(max_pt) or is_nan_pt(curve.back())
                 or (mapper.to<zg::pixel>(max_pt) - mapper.to<zg::pixel>(curve.back()))
                        .square_length()
                      > sq_px_step)))
      data.insert_pt(curve.size(), max, max_pt);
  }

  assert(curve.size() >= 2);

  size_t i = 0;

  const zg::real_unit smallest_allowed_step = data.get_smallest_allowed_step();
  const zg::real_unit biggest_allowed_step = data.get_biggest_allowed_step();

  while (i + 1 < input_vals.size())
  {
    const size_t i_b = i;
    const zg::real_unit& b = input_vals[i_b];
    const zg::real_pt& B = curve[i_b];
    const zg::pixel_pt px_B = mapper.to<zg::pixel>(B);

    const size_t i_c = i+1;
    const zg::real_unit& c = input_vals[i_c];
    const zg::real_pt& C = curve[i_c];
    const zg::pixel_pt px_C = mapper.to<zg::pixel>(C);

    const zg::real_unit bc = c - b;
    const zg::pixel_pt px_BC = px_C - px_B;
    const double px_sq_BC = px_BC.square_length();

    const bool nan_pt = is_nan_pt(B) or is_nan_pt(C);

    if (bc > biggest_allowed_step or nan_pt or px_sq_BC > sq_px_step)
    {
      if (bc >= 2*smallest_allowed_step)
      {
        const zg::real_unit mid_input = get_acceptable_input((c + b) / 2.);
        if constexpr (t == zg::CurveType::DISCRETE)
        {
          if (not (b < mid_input and mid_input < c))
          {
            i = i_c;
            continue;
          }
        }
        auto pt = get_f_pt(mid_input);
        data.insert_pt(i_c, mid_input, pt);
        i = i_b;
      }
      else
      {
        if constexpr (t == zg::CurveType::CONTINUOUS)
        {
          if (not nan_pt)
          {
            // try to figure out if there's a discontinuity

            bool is_discontinuity = true;
            if (i > 0)
            {
              const size_t i_a = i-1;
              const zg::real_pt& A = curve[i_a];
              const zg::pixel_pt px_A = mapper.to<zg::pixel>(A);

              const zg::pixel_pt px_AB = px_B - px_A;

              double dot = px_AB.dot(px_BC);
              if (sq_dist_to_segment(px_A, px_B, px_C) < 4 * sq_px_step and dot > 0
                  and dot * dot > 0.9 * px_AB.square_length() * px_BC.square_length())
                is_discontinuity = false;
            }
            if (is_discontinuity and i + 2 < input_vals.size())
            {
              const size_t i_d = i+2;
              const zg::real_pt& D = curve[i_d];
              const zg::pixel_pt px_D = mapper.to<zg::pixel>(D);

              const zg::pixel_pt px_CD = px_D - px_C;

              double dot = px_BC.dot(px_CD);
              if (sq_dist_to_segment(px_B, px_C, px_D) < 4 * sq_px_step and dot > 0
                  and dot * dot > 0.9 * px_BC.square_length() * px_CD.square_length())
                is_discontinuity = false;
            }

            if (is_discontinuity)
              data.discontinuities.insert(i_c);
          }
        }
        i = i_c;
      }
    }
    else i++;
  }

  qDebug() << "Object caching: " << obj_name << " curve has " << curve.size() << " points";

  if constexpr (t == zg::CurveType::CONTINUOUS)
  {
    qDebug() << "  and has " << data.discontinuities.size() << " discontinuities.";
    i = 1;
    for (size_t index : data.discontinuities)
    {
      if (index == 0 or is_nan_pt(curve[index - 1]) or is_nan_pt(curve[index]))
        continue;
      qDebug() << "- Discontinuity " << i << ": between ( " << curve[index - 1].x.v << ", "
               << curve[index - 1].y.v << " ) and (" << curve[index].x.v << ", " << curve[index].y.v
               << " )";
      i++;
    }
  }
}
