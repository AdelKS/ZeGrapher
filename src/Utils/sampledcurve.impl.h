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

  if constexpr (t == CurveType::CONTINUOUS)
  {
    auto handle_container = [&](auto& container)
    {
      std::vector<size_t> to_reinsert;
      for (auto it = container.begin(); it != container.end();)
      {
        if (from <= *it and *it < to)
          it = container.erase(it);
        else if (to <= *it)
        {
          to_reinsert.push_back((*it) - size);
          it = container.erase(it);
        }
        else ++it;
      }

      for (size_t i: to_reinsert) container.insert(i);
    };

    handle_container(discontinuities);
  }

  Q_ASSERT(input.size() == curve.size());

  input.erase(input.begin() + from, input.begin() + to);
  curve.erase(curve.begin() + from, curve.begin() + to);
}

template <CurveType t>
zg::real_unit SampledCurve<t>::get_biggest_allowed_step() const
{
  if constexpr (t == CurveType::DISCRETE)
    return std::max(std::round(style.range.amplitude() / double(min_size) / style.step) * style.step, style.step);
  else return style.range.amplitude() / double(min_size);
};

template <CurveType t>
zg::real_unit SampledCurve<t>::get_smallest_allowed_step() const
{
  if constexpr (t == CurveType::DISCRETE)
    return std::max(std::round(style.range.amplitude() / double(max_size) / style.step) * style.step, style.step);
  else return style.range.amplitude() / double(max_size);
};

template <CurveType t>
void SampledCurve<t>::insert_pt(size_t index, zg::real_unit x, zg::real_pt pt)
{
  curve.insert(curve.begin() + index, pt);
  input.insert(input.begin() + index, x);

  if constexpr (t == CurveType::CONTINUOUS)
  {
    std::vector<size_t> to_reinsert;
    to_reinsert.reserve(curve.size()/2);
    for (auto it = discontinuities.begin(); it != discontinuities.end();)
    {
      if (index <= *it)
      {
        to_reinsert.push_back((*it) + 1);
        it = discontinuities.erase(it);
      }
      else ++it;
    }

    for (size_t i: to_reinsert) discontinuities.insert(i);
  }
}

} // namespace zg
