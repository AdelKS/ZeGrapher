/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "viewmapper.h"

namespace zg {

void ZeViewMapper::zoomView(point<u<plane::pixel>> center, double ratio)
{
  x.zoomView(center.x, ratio);
  y.zoomView(center.y, ratio);
}

void ZeViewMapper::translateView(point<u<plane::pixel>> vec)
{
  x.translateView(vec.x);
  y.translateView(vec.y);
}

void ZeViewMapper::setAxesSettings(const ZeAxesSettings &axesSettings)
{
  x.setAxisSettings(axesSettings.x.axisType, axesSettings.x.logSettings.base);
  y.setAxisSettings(axesSettings.y.axisType, axesSettings.y.logSettings.base);
}

ZeViewMapper& ZeViewMapper::setGraphRect(const QRectF &rect)
{
  using pixel_unit = u<plane::pixel>;

  x.setRange(Range1D<pixel_unit>{.min = unit<pixel_unit>{double(rect.left())},
                                 .max = unit<pixel_unit>{double(rect.right())}});

  y.setRange(Range1D<pixel_unit>{.min = unit<pixel_unit>{double(rect.top())},
                                 .max = unit<pixel_unit>{double(rect.bottom())}});

  return *this;
}

void ZeViewMapper::setGraphRange(const real_range2d& range)
{
  x.setRange(range.x);
  y.setRange(range.y);
}

} // namespace zg
