#pragma once

#include <cstddef>
#include <unordered_set>
#include <vector>

#include "GraphDraw/axismapper.h"
#include "plotstyle.h"

namespace zg {

enum struct CurveType {DISCRETE, CONTINUOUS};

template <CurveType t>
struct SampledCurve;

using SampledCurveContinuous = SampledCurve<CurveType::CONTINUOUS>;
using SampledCurveDiscrete = SampledCurve<CurveType::DISCRETE>;

template <CurveType t>
struct SampledCurve {

  static constexpr size_t min_size = 1024;
  static constexpr size_t max_size = 16384;

  explicit SampledCurve(const PlotStyle& _st): style(_st) {}

  const std::vector<real_unit>& get_input() const { return input; };

  const std::vector<real_pt>& get_curve() const { return curve; };

  size_t size() const;

  void clear();

  /// @brief remove curve 'size' points from 'from' (included)
  void erase_chunk(size_t from, size_t size);

  void insert_pt(size_t index, real_unit input, real_pt pt);

  void pop_back(size_t pop_num);

  void pop_front(size_t pop_num);

    /// @brief returns the biggest step that can be made between two computed points
  real_unit get_biggest_allowed_step() const;

  /// @brief returns the smallest step that can be made between two computed points before giving up and looking for
  real_unit get_smallest_allowed_step() const;

  const PlotStyle& style;

  /// @brief the indices in 'curve' before which a discontinuity happened
  /// @note only exists in continuous curves
  [[no_unique_address]] std::conditional_t<t == CurveType::CONTINUOUS,
                                           std::unordered_set<size_t>,
                                           std::monostate> discontinuities = {};

protected:
  std::vector<real_unit> input;

  /// @brief the computed points of this object's curve
  std::vector<real_pt> curve = {};
};

} // namespace zg
