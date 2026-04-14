#pragma once

#include "sampledcurve.h"

namespace zg {

template <CurveType t>
void SampledCurve<t>::clear()
{
  input.clear();
  curve.clear();

  if constexpr (t == CurveType::CONTINUOUS)
    discontinuities.clear();
}

template <CurveType t>
size_t SampledCurve<t>::size() const
{
  assert(input.size() == curve.size());
  return curve.size();
}

template <CurveType t>
void SampledCurve<t>::pop_back(size_t pop_num)
{
  erase_chunk(curve.size() - pop_num, pop_num);
}

template <CurveType t>
void SampledCurve<t>::pop_front(size_t pop_num)
{
  erase_chunk(0, pop_num);
}

template <CurveType t>
void SampledCurve<t>::erase_chunk(size_t from, size_t size)
{
  if (size == 0)
    return;

  const size_t to = from + size;

  input.erase(input.begin() + from, input.begin() + to);
  curve.erase(curve.begin() + from, curve.begin() + to);
}

template <CurveType t>
zg::real_unit SampledCurve<t>::get_biggest_allowed_step() const
{
  if constexpr (t == CurveType::DISCRETE)
    return std::max(std::round(style.getRange().amplitude() / double(min_size) / style.getStep()) * style.getStep(), style.getStep());
  else return style.getRange().amplitude() / double(min_size);
};

template <CurveType t>
zg::real_unit SampledCurve<t>::get_smallest_allowed_step() const
{
  if constexpr (t == CurveType::DISCRETE)
    return std::max(std::round(style.getRange().amplitude() / double(max_size) / style.getStep()) * style.getStep(), style.getStep());
  else return style.getRange().amplitude() / double(max_size);
};

template <CurveType t>
void SampledCurve<t>::insert_chunk(size_t index, const std::vector<real_unit>& x, const std::vector<real_pt>& f_x)
{
  assert(x.size() == f_x.size());

  input.insert(input.begin() + index, x.begin(), x.end());
  curve.insert(curve.begin() + index, f_x.begin(), f_x.end());
  if constexpr (t == CurveType::CONTINUOUS)
    discontinuities.clear();
}

template <CurveType t>
void SampledCurve<t>::sparse_insert(std::vector<size_t> indices, std::vector<real_unit> x, std::vector<real_pt> f_x)
{
  assert(indices.size() <= curve.size()); // can only insert as many points as there are originally in  'curve'
  assert(std::ranges::adjacent_find(indices, std::greater_equal{}) == indices.end()); // strictly increasing (implies sorted + unique)
  assert(indices.size() == x.size() and x.size() == f_x.size());

  std::vector<real_pt> new_curve;
  new_curve.reserve(curve.size() + indices.size());

  std::vector<real_unit> new_input;
  new_input.reserve(curve.size() + indices.size());

  size_t curve_index = 0, indices_metaindex = 0;
  while (curve_index != curve.size() and indices_metaindex != indices.size())
  {
    if (indices[indices_metaindex] <= curve_index)
    {
      new_input.push_back(x[indices_metaindex]);
      new_curve.push_back(f_x[indices_metaindex]);
      indices_metaindex++;
    }
    else
    {
      new_input.push_back(input[curve_index]);
      new_curve.push_back(curve[curve_index]);
      curve_index++;
    }
  }

  new_input.insert(new_input.end(), input.begin() + curve_index, input.end());
  new_curve.insert(new_curve.end(), curve.begin() + curve_index, curve.end());

  curve = std::move(new_curve);
  input = std::move(new_input);
}

} // namespace zg
