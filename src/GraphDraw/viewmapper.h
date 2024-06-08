/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include <QObject>
#include <QPair>
#include <QRectF>
#include <QWidget>

#include "axismapper.h"
#include "structures.h"

namespace zg {
class ZeViewMapper
{
public:
  ZeAxisMapper<ZeAxisName::X> x;
  ZeAxisMapper<ZeAxisName::Y> y;

  explicit ZeViewMapper() = default;

  void zoomView(point<u<plane::pixel>> center, double ratio);
  void translateView(point<u<plane::pixel>> vec);

  template <plane p>
  Range2D<u<p>> getRange() const;

  template <plane p>
  ZeViewMapper& setRange(const Range2D<u<p>>& range);

  /// @note converts it to the range returned by getRange<plane::pixel>()
  ZeViewMapper& setGraphRect(const QRectF& rect);

  /// @note converts it to the range returned by getRange<plane::pixel>()
  QRectF getGraphRect() const;

  template <plane q, plane p>
  point<u<q>> to(point<u<p>> pt) const;

  void setGraphRange(const GraphRange& range);
  void setAxesSettings(const ZeAxesSettings& axesSettings);

protected:
  void enforceOrthonormality();

  bool orthonormal = false;
};



template <plane p>
Range2D<u<p>> ZeViewMapper::getRange() const
{
  return Range2D<u<p>>{x.getRange<p>(), y.getRange<p>()};
}

template <plane p>
ZeViewMapper& ZeViewMapper::setRange(const Range2D<u<p>>& range)
{
  x.setRange(range.x);
  y.setRange(range.y);
  return *this;
}

template <plane q, plane p>
point<u<q>> ZeViewMapper::to(point<u<p>> pt) const
{
  return point<u<q>>{x.to<q>(pt.x), y.to<q>(pt.y)};
}

} // namespace zg
