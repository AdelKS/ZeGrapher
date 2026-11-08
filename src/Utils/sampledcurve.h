/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <cstddef>
#include <unordered_set>
#include <vector>

#include <QList>
#include <QPointF>
#include <QPolygonF>

#include "MathObjects/BuildingBlocks/base.h"
#include "Utils/unit.h"
#include "curvestyle.h"

namespace zg {

struct SampledCurve {

  size_t size() const;

  void clear();

  /// @brief removes 'size' points, starting at index 'from'
  void erase_chunk(size_t from, size_t size);

  /// @brief inserts a chunk of points before 'index'
  /// @param index: the index to insert before
  /// @param x: the function inputs
  /// @param f_x: the points to insert, the function values at 'x'
  /// @param px_f_x: the same points in pixel units
  /// @note the point at 'index' moves right by x.size()
  /// @note x, f_x and px_f_x must have the same size
  void insert_chunk(size_t index,
                    const std::vector<real_unit>& x,
                    const std::vector<real_pt>& f_x,
                    const std::vector<QPointF>& px_f_x);

  /// @brief inserts points at several places in the curve
  /// @param indices: the indices to insert before, in the curve before any
  ///                 insertion. Must be strictly increasing: one point per index
  /// @param x: the function inputs
  /// @param f_x: the points to insert, the function values at 'x'
  /// @param px_f_x: the same points in pixel units
  void sparse_insert(const std::vector<size_t>& indices,
                     const std::vector<real_unit>& x,
                     const std::vector<real_pt>& f_x,
                     const std::vector<QPointF>& px_f_x);

  void sparse_delete(const std::vector<size_t>& indices);

  void pop_back(size_t pop_num);

  void pop_front(size_t pop_num);

  /// @brief the pieces of 'px_curve' between NaN points and discontinuities
  QList<QPolygonF> polylines() const;

  CurveStyle style = {};

  bool discrete = false;

  std::unordered_set<size_t> discontinuities = {};

  std::vector<real_unit> input = {};

  /// @brief the computed points of this object's curve
  std::vector<real_pt> curve = {};

  /// @brief 'curve' in pixel coordinates. The Sampler updates it after each pass
  std::vector<QPointF> px_curve = {};

  SamplingSettings settings = {};
};

} // namespace zg
