#pragma once

#include "Utils/unit.h"
#include "information.h"
#include "sampler.h"


template <zg::PlotStyle::CoordinateSystem coordinates, zg::CurveType t>
void Sampler::sample(auto handle, zg::SampledCurve<t>& data)
{
  auto get_acceptable_input = [&](zg::real_unit x)
  {
    if constexpr (t == zg::CurveType::DISCRETE)
      return std::round(std::max(0., x.v) / data.style.getStep().v) * data.style.getStep();
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

  auto& input_vals = data.input;
  auto& curve = data.curve;
  auto& px_curve = data.px_curve;

  const auto range = data.style.getRange();

  // clear from the right
  if (not input_vals.empty())
  {
    size_t vals_to_pop = 0;
    const auto max = get_acceptable_input(range.max);
    while (vals_to_pop < input_vals.size() and input_vals[input_vals.size() - vals_to_pop - 1] > max)
      vals_to_pop++;

    data.pop_back(vals_to_pop);

    vals_to_pop = 0;
    const auto min = get_acceptable_input(range.min);
    while (vals_to_pop < input_vals.size() and input_vals[vals_to_pop] < min)
      vals_to_pop++;

    data.pop_front(vals_to_pop);
  }

  px_curve.resize(curve.size());
  std::ranges::transform(curve, px_curve.begin(),
                         [&](const auto& pt) { return QPointF(mapper.to<zg::pixel>(pt)); });

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
    << " sampling range: min=" << QString::number(range.min.v, 'g', 14)
    << " max=" << QString::number(range.max.v, 'g', 14);

  const zg::real_unit min_step = data.get_smallest_allowed_step();
  const zg::real_unit max_step = data.get_biggest_allowed_step();

  auto uniform_sample = [&](zg::real_unit start, zg::real_unit end) {
    std::vector<QPointF> px_points;
    std::vector<zg::real_pt> points;
    std::vector<zg::real_unit> input;

    assert(start < end);
    const size_t reserve = std::max((end - start) / max_step, 0.0);
    points.reserve(reserve);
    input.reserve(reserve);
    px_points.reserve(reserve);

    for (auto pos = start ; pos <= end; pos += max_step)
    {
      input.push_back(pos);
      points.push_back(get_f_pt(pos));
      px_points.push_back(QPointF(mapper.to<zg::pixel>(points.back())));
    }

    return std::make_tuple(input, points, px_points);
  };

  if (curve.empty())
  {
    std::tie(input_vals, curve, px_curve) = uniform_sample(get_acceptable_input(range.min), range.max);
  }
  else
  {
    const auto min = get_acceptable_input(range.min);
    const auto min_pt = get_f_pt(min);
    if (input_vals.front() - min >= max_step
        or is_nan_pt(min_pt) or is_nan_pt(curve.front())
        or (mapper.to<zg::pixel>(min_pt) - mapper.to<zg::pixel>(curve.front())).square_length() > sq_px_step)
    {
      auto&& [x, f_x, px_f_x] = uniform_sample(min, input_vals.front());
      data.insert_chunk(0, x, f_x, px_f_x);
    }

    const auto max = get_acceptable_input(range.max);
    const auto max_pt = get_f_pt(max);
    if (max - input_vals.back() >= max_step
        or is_nan_pt(max_pt) or is_nan_pt(curve.back())
        or (mapper.to<zg::pixel>(max_pt) - mapper.to<zg::pixel>(curve.back())).square_length() > sq_px_step)
    {
      auto&& [x, f_x, px_f_x] = uniform_sample(input_vals.back(), max);
      data.insert_chunk(input_vals.size(), x, f_x, px_f_x);
    }
  }

  assert(curve.size() >= 2);

  static std::vector<size_t> indices;
  indices.reserve(data.max_size/2);

  static std::vector<zg::real_unit> x;
  x.reserve(data.max_size/2);

  static std::vector<zg::real_pt> f_x;
  f_x.reserve(data.max_size/2);

  static std::vector<QPointF> px_f_x;
  px_f_x.reserve(data.max_size/2);

  do
  {
    indices.clear();
    x.clear();
    f_x.clear();
    px_f_x.clear();

    for (size_t i = 0 ; i + 1 < input_vals.size() ; i++)
    {
      const size_t i_b = i;
      const zg::real_unit& b = input_vals[i_b];
      const zg::real_pt& B = curve[i_b];
      const QPointF& px_B = px_curve[i_b];

      const size_t i_c = i+1;
      const zg::real_unit& c = input_vals[i_c];
      const zg::real_pt& C = curve[i_c];
      const QPointF& px_C = px_curve[i_c];

      const zg::real_unit bc = c - b;
      const QPointF px_BC = px_C - px_B;
      const double px_sq_BC = QPointF::dotProduct(px_BC, px_BC);

      const bool nan_pt = is_nan_pt(B) or is_nan_pt(C);

      if (bc > max_step or nan_pt or px_sq_BC > sq_px_step)
      {
        if (bc >= 2*min_step)
        {
          const zg::real_unit mid_input = get_acceptable_input((c + b) / 2.);
          if constexpr (t == zg::CurveType::DISCRETE)
          {
            if (not (b < mid_input and mid_input < c))
              continue;
          }
          const auto new_pt = get_f_pt(mid_input);
          indices.push_back(i_c);
          x.push_back(mid_input);
          f_x.push_back(new_pt);
          px_f_x.push_back(QPointF(mapper.to<zg::pixel>(new_pt)));
        }
      }
    }

    if (not indices.empty())
      data.sparse_insert(indices, x, f_x, px_f_x);

  } while(not indices.empty());

  qDebug() << "Object caching: " << obj_name << " curve has " << curve.size() << " points";
}
