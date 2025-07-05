#include "funccurve.h"

namespace zg {

void FuncCurve::clear()
{
  input.clear();
  curve.clear();
  discontinuities.clear();
}

size_t FuncCurve::size() const
{
  assert(input.size() == curve.size());
  return curve.size();
}

void FuncCurve::pop_back(size_t pop_num)
{
  erase_chunk(curve.size() - pop_num, pop_num);
}

void FuncCurve::pop_front(size_t pop_num)
{
  erase_chunk(0, pop_num);
}

void FuncCurve::erase_chunk(size_t from, size_t size)
{
  if (size == 0)
    return;

  const size_t to = from + size;

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

  Q_ASSERT(input.size() == curve.size());

  input.erase(input.begin() + from, input.begin() + to);
  curve.erase(curve.begin() + from, curve.begin() + to);
}

zg::real_unit FuncCurve::get_biggest_allowed_step() const
{
  return style.range.amplitude() / double(min_size);
};

zg::real_unit FuncCurve::get_smallest_allowed_step() const
{
  return style.range.amplitude() / double(max_size);
};

void FuncCurve::insert_pt(size_t index, zg::real_unit x, zg::real_pt pt)
{
  curve.insert(curve.begin() + index, pt);
  input.insert(input.begin() + index, x);

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

} // namespace zg
