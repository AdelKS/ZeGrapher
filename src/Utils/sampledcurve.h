#pragma once

#include <cstddef>
#include <unordered_set>
#include <vector>

#include <QPointF>

#include "MathObjects/mathobject.h"
#include "Utils/unit.h"
#include "curvestyle.h"

namespace zg {

struct SampledCurve {

  size_t size() const;

  void clear();

  /// @brief remove curve 'size' points starting from 'from' (included)
  void erase_chunk(size_t from, size_t size);

  /// @brief inserts a chunk of points before 'index'
  /// @param index: at which index to insert before
  /// @param x: input values to the sampled function
  /// @param f_x: points to insert, image of 'x' by the function
  /// @param px_f_x: points to insert in pixel units
  /// @note old 'index' is moved by x.size() positions
  /// @note input.size() == pts.size() is required
  void insert_chunk(size_t index,
                    const std::vector<real_unit>& x,
                    const std::vector<real_pt>& f_x,
                    const std::vector<QPointF>& px_f_x);

  /// @brief inserts a set of points into the existing dataset
  /// @param indices: at what indices to insert before, relative to _current_ curve state before any insertion happens
  ///                 **MUST** be sorted in a strictly increasing order (can only insert once at a given index)
  /// @param x: input values to the sampled function
  /// @param f_x: points to insert, image of 'x' by the function
  /// @param px_f_x: points to insert in pixel units
  void sparse_insert(const std::vector<size_t>& indices,
                     const std::vector<real_unit>& x,
                     const std::vector<real_pt>& f_x,
                     const std::vector<QPointF>& px_f_x);

  void sparse_delete(const std::vector<size_t>& indices);

  void pop_back(size_t pop_num);

  void pop_front(size_t pop_num);

  CurveStyle style = {};

  bool discrete = false;

  std::unordered_set<size_t> discontinuities = {};

  std::vector<real_unit> input = {};

  /// @brief the computed points of this object's curve
  std::vector<real_pt> curve = {};

  /// @brief pixel coordinates of 'curve', updated by the Sampler after each sampling pass
  std::vector<QPointF> px_curve = {};

  SamplingSettings settings = {};
};

} // namespace zg
