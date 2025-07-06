#pragma once

#include "information.h"
#include "sampler.h"

/// @brief returns the square distance between B and the segment [AB]
inline double sq_dist_to_segment(const zg::pixel_pt& A, const zg::pixel_pt& P, const zg::pixel_pt& B) {
  const zg::pixel_pt AB = B - A;
  const zg::pixel_pt AP = P - A;
  const double t = std::max(0., std::min(AP.dot(AB) / AB.square_length(), 1.));
  return (AP - t * AB).square_length();
};

template <zg::CurveType t>
void Sampler::sample(const zg::MathObject&f, zg::SampledCurve<t>& data)
{
  auto get_f_pt = [&f](zg::real_unit x)
  {
    return f(x, &information.mathObjectCache);
  };

  const double sq_px_step = pixelStep.v * pixelStep.v;

  const std::vector<zg::real_unit>& input_vals = data.get_input();
  const std::vector<zg::real_pt>& curve = data.get_curve();

  // clear from the right
  if (not input_vals.empty())
  {
    size_t vals_to_pop = 0;
    while (input_vals[input_vals.size() - vals_to_pop - 1] > data.style.range.max)
      vals_to_pop++;

    data.pop_back(vals_to_pop);

    vals_to_pop = 0;
    while (input_vals[vals_to_pop] < data.style.range.min)
      vals_to_pop++;

    data.pop_front(vals_to_pop);
  }

  qDebug() << "Object caching: " << f.getName()
    << " sampling range: min=" << QString::number(data.style.range.min.v, 'g', 14)
    << " max=" << QString::number(data.style.range.max.v, 'g', 14);

  {
    auto min_pt = get_f_pt(data.style.range.min);
    if (curve.empty()
        or (mapper.to<zg::pixel>(min_pt) - mapper.to<zg::pixel>(curve.front())).square_length()
            > sq_px_step)
      data.insert_pt(0, data.style.range.min, min_pt);
  }
  {
    auto max_pt = get_f_pt(data.style.range.max);
    if (curve.size() == 1
        or (mapper.to<zg::pixel>(max_pt) - mapper.to<zg::pixel>(curve.back())).square_length()
            > sq_px_step)
      data.insert_pt(curve.size(), data.style.range.max, max_pt);
  }

  assert(curve.size() >= 2);

  size_t i = 0;

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

    if (px_BC.square_length() > sq_px_step)
    {
      if (bc > data.get_smallest_allowed_step())
      {
        const zg::real_unit mid_input = (c + b) / 2.;
        auto pt = get_f_pt(mid_input);
        data.insert_pt(i_c, mid_input, pt);
        i = i_b;
      }
      else
      {
        if constexpr (t == zg::CurveType::CONTINUOUS)
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

          i = i_c;
        }
        else
        {
          i++;
        }
      }
    }
    else i++;
  }

  qDebug() << "Object caching: " << f.getName() << " curve has " << curve.size() << " points";

  if constexpr (t == zg::CurveType::CONTINUOUS)
  {
    qDebug() << "Object caching: " << f.getName() << " has " << data.discontinuities.size() << " discontinuities.";
    i = 1;
    for (size_t index : data.discontinuities)
    {
      qDebug() << "- Discontinuity " << i << ": between ( " << data.get_curve()[index-1].x.v << ", " << data.get_curve()[index-1].y.v << " ) and (" << data.get_curve()[index].x.v << ", " << data.get_curve()[index].y.v << " )";
      i++;
    }
  }
}
