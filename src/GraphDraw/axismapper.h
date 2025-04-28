/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include "structures.h"
#include "Utils/unit.h"
#include <zecalculator/utils/utils.h>

namespace zg {

enum plane {
  real, view, pixel
};

template <plane t>
struct u {};

template <plane t>
struct is_unit_type<u<t>>: std::true_type {};

using view_unit = unit<u<plane::view>>;
using pixel_unit = unit<u<plane::pixel>>;
using real_unit = unit<u<plane::real>>;

using view_pt = point<u<plane::view>>;
using pixel_pt = point<u<plane::pixel>>;
using real_pt = point<u<plane::real>>;

using view_range1d = Range1D<u<plane::view>>;
using pixel_range1d = Range1D<u<plane::pixel>>;
using real_range1d = Range1D<u<plane::real>>;

using view_range2d = Range2D<u<plane::view>>;
using pixel_range2d = Range2D<u<plane::pixel>>;
using real_range2d = Range2D<u<plane::real>>;

template <ZeAxisName axis>
class ZeAxisMapper
{
public:
  explicit ZeAxisMapper() = default;

  /// @brief zoom view, centered on center
  void zoomView(unit<u<plane::pixel>> center, double ratio);

  /// @brief zoom view, centered on the middle of the range
  void zoomView(double ratio);

  void translateView(unit<u<plane::pixel>> vec);

  /// @brief sets the type of mapping
  /// @param type: LINEAR or LOG
  /// @param base: LOG base, only used in LOG type
  void setAxisSettings(ZeViewType type, double base);

  template <plane p>
  const Range1D<u<p>>& getRange() const;

  template <plane p>
  void setRange(const Range1D<u<p>>& range);

  template <plane p>
  void setMin(unit<u<p>> min);

  template <plane p>
  unit<u<p>> getMin() const;

  template <plane p>
  void setMax(unit<u<p>> max);

  template <plane p>
  unit<u<p>> getMax() const;

  template <plane p>
  bool isInView(unit<u<p>> x) const;

  template <plane q, plane p>
  unit<u<q>> to(unit<u<p>> x) const;

protected:
  template <plane p>
  Range1D<u<p>>& getRange();

  Range1D<u<plane::real>> real_range;
  Range1D<u<plane::view>> view_range;

  /// @brief the actual graph rect
  Range1D<u<plane::pixel>> pixel_range;

  double base = 0;
  ZeViewType view_type = ZeViewType::LINEAR;
};

template <ZeAxisName axis>
template <plane p>
Range1D<u<p>>& ZeAxisMapper<axis>::getRange()
{
  return const_cast<Range1D<u<p>>&>(std::as_const(*this).template getRange<p>());
}

template <ZeAxisName axis>
template <plane p>
const Range1D<u<p>>& ZeAxisMapper<axis>::getRange() const
{
  if constexpr (p == plane::view)
    return view_range;
  else if constexpr (p == plane::pixel)
    return pixel_range;
  else return real_range;
}

template <ZeAxisName axis>
template <plane p>
unit<u<p>> ZeAxisMapper<axis>::getMin() const
{
  return getRange<p>().min;
}

template <ZeAxisName axis>
template <plane p>
unit<u<p>> ZeAxisMapper<axis>::getMax() const
{
  return getRange<p>().max;
}

template <ZeAxisName axis>
template <plane p>
bool ZeAxisMapper<axis>::isInView(unit<u<p>> x) const
{
  return getRange<p>().min <= x and x <= getRange<p>().max;
}

template <ZeAxisName axis>
template <plane p>
void ZeAxisMapper<axis>::setRange(const Range1D<u<p>>& range)
{
  getRange<p>() = range;

  if constexpr (p == plane::real or p == plane::view)
  {
    constexpr plane q = p == plane::real ? plane::view : plane::real;
    switch (view_type)
    {
      case ZeViewType::LINEAR:
        getRange<q>().min.v = range.min.v;
        getRange<q>().max.v = range.max.v;
        break;

      case ZeViewType::LOG:
        getRange<q>().min = to<q>(range.min);
        getRange<q>().max = to<q>(range.max);
        break;
    }
  }
}

template <ZeAxisName axis>
template <plane p>
void ZeAxisMapper<axis>::setMin(unit<u<p>> min)
{
  auto range = getRange<p>();
  range.min = min;

  setRange(range);
}

template <ZeAxisName axis>
template <plane p>
void ZeAxisMapper<axis>::setMax(unit<u<p>> max)
{
  auto range = getRange<p>();
  range.max = max;

  setRange(range);
}

template <ZeAxisName axis>
template <plane q, plane p>
unit<u<q>> ZeAxisMapper<axis>::to(unit<u<p>> x) const
{
  if constexpr (p == q)
    return x;

  else if constexpr ((p == plane::pixel and q == plane::real)
                     or (p == plane::real and q == plane::pixel))
  {
    // go through the "view" intermediate step
    return to<q>(to<plane::view>(x));
  }

  else if constexpr ((p == plane::pixel and q == plane::view)
                     or (p == plane::view and q == plane::pixel))
  {
    const auto& orig = getRange<p>();
    const auto& dest = getRange<q>();

    if constexpr (axis == ZeAxisName::X)
      return (x - orig.min) / orig.amplitude() * dest.amplitude() + dest.min;
    else return (orig.max - x) / orig.amplitude() * dest.amplitude() + dest.min; // flipped
  }

  else
  {
    static_assert((p == plane::real and q == plane::view) or (p == plane::view and q == plane::real), "something wrong");
    unit<u<q>> result;
    switch (view_type)
    {
      case ZeViewType::LINEAR:
        result = unit<u<q>>{x.v}; // no scaling
        break;

      case ZeViewType::LOG:
        if constexpr (p == plane::real)
        {
          Q_ASSERT(x.v > 0);
          result = unit<u<q>>{std::log(x.v) / std::log(base)};
        }
        else result = unit<u<q>>{std::pow(base, x.v)};
        break;
    }
    return result;
  }
}

template <ZeAxisName axis>
void ZeAxisMapper<axis>::zoomView(unit<u<plane::pixel>> center, double ratio)
{
  Q_ASSERT(pixel_range.contains(center));

  const double new_amplitude = ratio * view_range.amplitude().v;

  const auto view_center = to<plane::view>(center);

  if(MAX_AMPLITUDE > new_amplitude and new_amplitude > MIN_AMPLITUDE)
    setRange(
      Range1D<u<plane::view>>{.min = view_range.min - (1 - ratio) * (view_range.min - view_center),
                              .max = view_range.max - (1 - ratio) * (view_range.max - view_center)});
}

template <ZeAxisName axis>
void ZeAxisMapper<axis>::zoomView(double ratio)
{
  zoomView(pixel_range.min + pixel_range.amplitude()/2, ratio);
}

template <ZeAxisName axis>
void ZeAxisMapper<axis>::translateView(unit<u<plane::pixel>> vec)
{
  if constexpr (axis == ZeAxisName::Y)
    vec = -vec;

  setRange(view_range.translated(vec / pixel_range.amplitude() * view_range.amplitude()));
  pixel_range.translate(vec);
}

template <ZeAxisName axis>
void ZeAxisMapper<axis>::setAxisSettings(ZeViewType type, double base)
{
  view_type = type;
  this->base = base;

  setRange(view_range);
}

}
