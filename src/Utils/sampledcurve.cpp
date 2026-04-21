#include "sampledcurve.h"

namespace zg {

void SampledCurve::clear()
{
  input.clear();
  curve.clear();
  px_curve.clear();
  discontinuities.clear();
}

size_t SampledCurve::size() const
{
  assert(input.size() == curve.size());
  return curve.size();
}

void SampledCurve::pop_back(size_t pop_num)
{
  erase_chunk(curve.size() - pop_num, pop_num);
}

void SampledCurve::pop_front(size_t pop_num)
{
  erase_chunk(0, pop_num);
}

void SampledCurve::erase_chunk(size_t from, size_t size)
{
  assert(input.size() == curve.size());
  assert(px_curve.size() == input.size());
  assert(from + size <= curve.size());

  if (size == 0)
    return;

  const size_t to = from + size;

  input.erase(input.begin() + from, input.begin() + to);
  curve.erase(curve.begin() + from, curve.begin() + to);
  px_curve.erase(px_curve.begin() + from, px_curve.begin() + to);
}

zg::real_unit SampledCurve::get_biggest_allowed_step() const
{
  if (discrete)
    return std::max(std::round(settings.range.amplitude() / double(min_size) / settings.step) * settings.step, settings.step);
  else return settings.range.amplitude() / double(min_size);
};

zg::real_unit SampledCurve::get_smallest_allowed_step() const
{
  if (discrete)
    return std::max(std::round(settings.range.amplitude() / double(max_size) / settings.step) * settings.step, settings.step);
  else return settings.range.amplitude() / double(max_size);
};

void SampledCurve::insert_chunk(size_t index,
                                const std::vector<real_unit>& x,
                                const std::vector<real_pt>& f_x,
                                const std::vector<QPointF>& px_f_x)
{
  assert(x.size() == f_x.size());

  input.insert(input.begin() + index, x.begin(), x.end());
  curve.insert(curve.begin() + index, f_x.begin(), f_x.end());
  px_curve.insert(px_curve.begin() + index, px_f_x.begin(), px_f_x.end());
  discontinuities.clear();
}

void SampledCurve::sparse_insert(const std::vector<size_t>& indices,
                                 const std::vector<real_unit>& x,
                                 const std::vector<real_pt>& f_x,
                                 const std::vector<QPointF>& px_f_x)
{
  assert(indices.size() <= curve.size()); // can only insert as many points as there are originally in  'curve'
  assert(std::ranges::adjacent_find(indices, std::greater_equal{}) == indices.end()); // strictly increasing (implies sorted + unique)
  assert(indices.size() == x.size() and x.size() == f_x.size() and f_x.size() == px_f_x.size());

  const size_t new_size = curve.size() + indices.size();

  std::vector<real_unit> new_input;
  new_input.reserve(new_size);

  std::vector<real_pt> new_curve;
  new_curve.reserve(new_size);

  std::vector<QPointF> new_px_curve;
  new_px_curve.reserve(new_size);

  size_t curve_index = 0, indices_metaindex = 0;
  while (curve_index != curve.size() and indices_metaindex != indices.size())
  {
    if (indices[indices_metaindex] <= curve_index)
    {
      new_input.push_back(x[indices_metaindex]);
      new_curve.push_back(f_x[indices_metaindex]);
      new_px_curve.push_back(px_f_x[indices_metaindex]);
      indices_metaindex++;
    }
    else
    {
      new_input.push_back(input[curve_index]);
      new_curve.push_back(curve[curve_index]);
      new_px_curve.push_back(px_curve[curve_index]);
      curve_index++;
    }
  }

  new_input.insert(new_input.end(), input.begin() + curve_index, input.end());
  new_curve.insert(new_curve.end(), curve.begin() + curve_index, curve.end());
  new_px_curve.insert(new_px_curve.end(), px_curve.begin() + curve_index, px_curve.end());

  input = std::move(new_input);
  curve = std::move(new_curve);
  px_curve = std::move(new_px_curve);
}

void SampledCurve::update_sampling_settings(const zg::MathObject& obj, const zg::PlotStyle& plotStyle)
{
  SamplingSettings newSettings = {
    .range = plotStyle.getRange(),
    .step = plotStyle.getStep(),
    .coordinateSystem = plotStyle.coordinateSystem,
    .revision = obj.getRevision()
  };

  if (newSettings != settings)
  {
    clear();
    settings = newSettings;
  }
}

} // namespace zg
