#pragma once

#include "Utils/unit.h"
#include "information.h"
#include "sampler.h"


template <zg::PlotStyle::CoordinateSystem coordinates, bool discrete>
void Sampler::sample(auto handle, zg::SampledCurve& data)
{
  auto get_acceptable_input = [&](zg::real_unit x)
  {
    if constexpr (discrete)
      return std::round(std::max(0., x.v) / data.settings.step.v) * data.settings.step;
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

  auto is_nan_pt = []<typename T>(const T& pt)
  {
    if constexpr (std::is_same_v<T, zg::real_pt>)
      return std::isnan(pt.x.v) or std::isnan(pt.y.v);
    else if constexpr (std::is_same_v<T, QPointF>)
      return std::isnan(pt.x()) or std::isnan(pt.y());
    else static_assert(zc::utils::dependent_false_v<T>, "Type not handled");
  };

  // only used in debug builds
  [[maybe_unused]] auto is_unique = [](const auto& vals) {
    for (auto it = vals.begin() ; it != vals.end() and it+1 != vals.end() ; it++)
      if (*it == *(it+1))
        return false;
    return true;
  };

  const auto range = data.settings.range;
  const auto min = get_acceptable_input(range.min);
  const auto max = get_acceptable_input(range.max);

  // increase cache size for sequences to max 1M values so we can
  // pan anywhere with recursive sequences, but if the cache gets
  // invalidated we're back to square 1
  auto increase_seq_cache = [&](const zc::DynMathObject<zc_t>* f){
    if (f->holds(zc::ObjectType::SEQUENCE))
    {
      auto& objectCache = information.mathObjectCache[f->get_slot()];
      size_t current_max = std::max(0., max.v);
      objectCache.set_buffer_size(std::min(std::max(current_max, objectCache.get_buffer_size()), 1'000'000UL));
    }
  };

  zc::utils::overloaded{
    [&](const zc::DynMathObject<zc_t>* f)
    {
      increase_seq_cache(f);
    },
    [&](std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*> f)
    {
      increase_seq_cache(f.first);
      increase_seq_cache(f.second);
    },
  }(handle);

  auto& input_vals = data.input;
  auto& curve = data.curve;
  auto& px_curve = data.px_curve;

  // clear from the right
  if (not input_vals.empty())
  {
    size_t vals_to_pop = 0;

    while (vals_to_pop < input_vals.size() and input_vals[input_vals.size() - vals_to_pop - 1] > max)
      vals_to_pop++;

    data.pop_back(vals_to_pop);

    vals_to_pop = 0;
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

  const zg::real_unit max_step = data.get_biggest_allowed_step();

  // add uniformly sampled points
  auto uniform_sample = [&](zg::real_unit start, zg::real_unit end) {
    std::vector<QPointF> px_points;
    std::vector<zg::real_pt> points;
    std::vector<zg::real_unit> input;

    assert(start <= end);
    const size_t reserve = std::max((end - start) / max_step, 0.0);
    points.reserve(reserve);
    input.reserve(reserve);
    px_points.reserve(reserve);

    for (auto pos = start; pos <= end; pos += max_step)
    {
      input.push_back(pos);
      points.push_back(get_f_pt(pos));
      px_points.push_back(QPointF(mapper.to<zg::pixel>(points.back())));
    }

    return std::make_tuple(input, points, px_points);
  };

  if (curve.empty())
  {
    std::tie(input_vals, curve, px_curve) = uniform_sample(get_acceptable_input(range.min - max_step), range.max + max_step);
  }
  else
  {
    const auto min = get_acceptable_input(range.min);
    if (input_vals.front() - min >= max_step)
    {
      auto&& [x, f_x, px_f_x] = uniform_sample(min - max_step, input_vals.front() - max_step);
      data.insert_chunk(0, x, f_x, px_f_x);
      assert(std::ranges::is_sorted(input_vals));
      assert(is_unique(input_vals));
    }

    const auto max = get_acceptable_input(range.max);
    if (max - input_vals.back() >= max_step)
    {
      auto&& [x, f_x, px_f_x] = uniform_sample(input_vals.back() + max_step, max + max_step);
      data.insert_chunk(input_vals.size(), x, f_x, px_f_x);
      assert(std::ranges::is_sorted(input_vals));
      assert(is_unique(input_vals));
    }
  }
  // ######################################

  static std::vector<size_t> indices;
  indices.reserve(data.max_size/2);

  static std::vector<zg::real_unit> x;
  x.reserve(data.max_size/2);

  static std::vector<zg::real_pt> f_x;
  f_x.reserve(data.max_size/2);

  static std::vector<QPointF> px_f_x;
  px_f_x.reserve(data.max_size/2);

  const zg::real_unit min_input_dist = range.amplitude() / double(data.max_size);

  // refine if points are too far apart
  // so when we compute 3 points A, B, C with 3 consecutive input values,
  // we should be at a refinement level where
  // -  AB.AC > 0: C follows the trend that is shown by A and B and
  //    doesn't "jump"
  // - A, B and C are "nearly" aligned
  // --> we check that sq_dist_to_ray(px_A, px_B, px_C) < sq_dist_to_ray_limit
  //     i.e. the distance of C to the ray [AB) is less than a pixel, less than 'sq_dist_to_ray_limit'
  do
  {
    indices.clear();
    x.clear();
    f_x.clear();
    px_f_x.clear();

    for (size_t i = 0 ; i + refine_increment < input_vals.size() ; i += refine_increment)
    {
      const size_t i_b = i+1;
      const size_t i_c = i+2;

      auto refine = [&]() -> std::optional<zg::real_unit> {
        const size_t i_a = i;
        const zg::real_unit& a = input_vals[i_a];
        const zg::real_pt& A = curve[i_a];
        const QPointF& px_A = px_curve[i_a];

        const zg::real_unit& b = input_vals[i_b];
        const zg::real_pt& B = curve[i_b];
        const QPointF& px_B = px_curve[i_b];

        bool nan_pt = is_nan_pt(A) or is_nan_pt(B);

        const QPointF px_AB = px_A - px_B;
        const double sq_px_AB = QPointF::dotProduct(px_AB, px_AB);

        if (sq_px_AB < 0.125)
          return {};

        if constexpr (discrete)
        {
          const auto ba = b - a;
          if (ba < min_input_dist)
            return {};

          if (nan_pt or sq_px_AB >= 2*min_sq_dist_discrete)
          {
            auto mid = get_acceptable_input((a + b) / 2.);
            if (a < mid and mid < b)
              return mid;
            else return {};
          }
          else return {};
        }
        else
        {
          const zg::real_unit& c = input_vals[i_c];
          const zg::real_pt& C = curve[i_c];
          const QPointF& px_C = px_curve[i_c];

          const zg::real_unit bc = c - b;
          if (bc < min_input_dist)
            return {};

          nan_pt = nan_pt or is_nan_pt(C);

          if (bc > max_step or nan_pt or sq_dist_to_ray(px_A, px_B, px_C) >= sq_dist_to_ray_limit)
          {
            auto mid = get_acceptable_input((b + c) / 2.);
            assert(b < mid and mid < c);
            return mid;
          }
          else return {};
        }
      };

      if (auto opt_mid_input = refine())
      {
        const zg::real_unit mid_input = *opt_mid_input;

        const auto new_pt = get_f_pt(mid_input);
        indices.push_back(discrete ? i_b : i_c);
        x.push_back(mid_input);
        f_x.push_back(new_pt);
        px_f_x.push_back(QPointF(mapper.to<zg::pixel>(new_pt)));
      }
    }

    if (not indices.empty())
      data.sparse_insert(indices, x, f_x, px_f_x);

    assert(std::ranges::is_sorted(input_vals));
    assert(is_unique(input_vals));

  } while(not indices.empty() and data.size() < data.max_size);
  // ######################################

  qDebug() << "Object caching: " << obj_name << " curve has " << curve.size() << " points";
}
