/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <zecalculator/utils/utils.h>
#include <QPointF>
#include "structures.h"

namespace zg {

template <class T>
struct is_unit_type: std::false_type {};

template <class T>
inline constexpr bool is_unit_type_v = is_unit_type<T>::value;

template <class T>
concept UnitType = is_unit_type_v<T>;

template <UnitType U>
struct unit
{
  double v = {};

  unit operator + (unit other) const {return unit{v + other.v}; }
  unit operator - (unit other) const {return unit{v - other.v}; }

  unit& operator += (unit other) { v += other.v; return *this; }
  unit& operator -= (unit other) { v -= other.v; return *this; }

  double operator / (unit div) const { return v / div.v; }

  unit operator / (double div) const { return unit{v / div}; }
  unit& operator /= (double div) { v /= div; return *this; }

  unit& operator *= (double mul) { v *= mul; return *this; }

  unit operator - () const { return unit{-v};}

  unit length() const { return unit{std::fabs(v)}; }

  auto operator <=> (unit other) const { return v <=> other.v; }
};

template <UnitType U>
unit<U> operator * (double mul, unit<U> x) { return unit<U>{x.v * mul}; }

template <UnitType U>
struct Range1D
{
  unit<U> min = {}, max = {};

  unit<U> amplitude() const { return max - min; }
  Range1D& translate(unit<U> vec) { min += vec; max += vec; return *this; }
  Range1D translated(unit<U> vec) const { return Range1D{.min = min + vec, .max = max + vec}; }
  bool contains(unit<U> x) const { return min <= x and x <= max; }
  std::optional<Range1D> intersection(const Range1D& other)
  {
    if (other.max <= min or max <= other.min)
      return {};
    else
      return Range1D{.min = std::max(min, other.min), .max = std::min(max, other.max)};
  }
};

template <UnitType U>
struct point
{
  unit<U> x = {}, y = {};

  point& translate(point vec) { x += vec.x; y *= vec.y; return *this; }
  point translated(point vec) const { return point{.x = x + vec, .y = x + vec}; }

  double square_length() const { return x.v*x.v + y.v*y.v; };

  static point from(const QPointF& pt)
  {
    return point{unit<U>{pt.x()}, unit<U>{pt.y()}};
  }

  static point from(const Point& pt)
  {
    return point{unit<U>{pt.x}, unit<U>{pt.y}};
  }

  explicit operator QPointF () const
  {
    return QPointF(x.v, y.v);
  }
};

template <UnitType U>
point<U> operator - (const point<U>& a, const point<U>& b)
{
  return point<U>{.x = a.x - b.x, .y = a.y - b.y};
}

template <UnitType U>
point<U> operator + (const point<U>& a, const point<U>& b)
{
  return point<U>{.x = a.x + b.x, .y = a.y + b.y};
}

template <UnitType U>
struct Range2D
{
  Range1D<U> x = {}, y = {};

  point<U> amplitude() const { return point<U>{.x = x.amplitude(), .y = y.amplitude()}; }
  Range2D& translate(point<U> vec) { x.translate(vec.x); y.translate(vec.y); return *this; }
  Range2D translated(point<U> vec) const { return Range1D{.x = x.translated(vec.x), .y = y.translated(vec.y)}; }
  bool contains(point<U> pt) const { return x.contains(pt.x) and y.contains(pt.y); }

};

}
