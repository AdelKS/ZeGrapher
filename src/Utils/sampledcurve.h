#pragma once

#include <cstddef>
#include <unordered_set>
#include <vector>

#include <QPointF>

#include "GraphDraw/axismapper.h"
#include "MathObjects/mathobject.h"
#include "curvestyle.h"
#include "plotstyle.h"

namespace zg {

struct SampledCurve {

  static constexpr size_t min_size = 128;
  static constexpr size_t max_size = 16384;

  SampledCurve() = default;

  const std::vector<real_unit>& get_input() const { return input; };

  const std::vector<real_pt>& get_curve() const { return curve; };

  size_t size() const;

  void clear();

  /// @brief remove curve 'size' points starting from 'from' (included)
  void erase_chunk(size_t from, size_t size);

  /// @brief inserts a chunk of points before 'index'
  /// @param index: at which index to insert before
  /// @param x: the input values to the function
  /// @param f_x: the points to insert, image of 'input' by the function
  /// @param px_f_x: to points to insert in pixel units
  /// @note old index is moved by input.size() positions
  /// @note input.size() == pts.size() is required
  void insert_chunk(size_t index,
                    const std::vector<real_unit>& x,
                    const std::vector<real_pt>& f_x,
                    const std::vector<QPointF>& px_f_x);

  /// @brief inserts a set of points into the existing dataset
  /// @param indices: at what indices to insert before, relative to _current_ curve state before any insertion happens
  ///                 **MUST** be sorted in increasing order, can only insert once at _any_ index
  ///                 e.g. if curve.size() == 2, one can only insert 3 points, at indices [0,1,2]
  /// @param x: the input value that was used to compute each point at the same index in 'pts'
  /// @param f_x: the points to insert, image of 'input' by the function
  void sparse_insert(const std::vector<size_t>& indices,
                     const std::vector<real_unit>& x,
                     const std::vector<real_pt>& f_x,
                     const std::vector<QPointF>& px_f_x);

  void sparse_delete(const std::vector<size_t>& indices);

  void pop_back(size_t pop_num);

  void pop_front(size_t pop_num);

    /// @brief returns the biggest step that can be made between two computed points
  real_unit get_biggest_allowed_step() const;

  /// @brief returns the smallest step that can be made between two computed points before giving up and looking for
  real_unit get_smallest_allowed_step() const;

  /// @brief
  void update_sampling_settings(const zg::MathObject& obj, const zg::PlotStyle& plotStyle);

  CurveStyle style;

  bool discrete = false;

  std::unordered_set<size_t> discontinuities;

  std::vector<real_unit> input;

  /// @brief the computed points of this object's curve
  std::vector<real_pt> curve = {};

  /// @brief pixel coordinates of 'curve', updated by the Sampler after each sampling pass
  std::vector<QPointF> px_curve = {};

  /// @brief settings that affect the resulting sampled values
  struct SamplingSettings {
    zg::real_range1d range = {.min = {0.}, .max = {0.}};
    zg::real_unit step = {0.};
    zg::PlotStyle::CoordinateSystem coordinateSystem = zg::PlotStyle::Cartesian;
    size_t revision = 0;

    bool operator == (const SamplingSettings&) const = default;
  } settings;
};

} // namespace zg
