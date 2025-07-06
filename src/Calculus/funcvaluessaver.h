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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include "GraphDraw/viewmapper.h"
#include "Utils/sampledcurve.h"

#include <zecalculator/zecalculator.h>

namespace zg {
  struct PlotStyle;
}

class FuncValuesSaver
{
public:
  FuncValuesSaver(const zg::ZeViewMapper& mapper, double pxStep);

  void setPixelStep(double pxStep);
  void update();

  const auto& getCurves() { return curves; }

  /// @brief clears the saved points of functions whose name is in 'objectNames'
  void clearCache(QStringList objectNames);

  /// @brief refreshes 'funcCurves' for the currently valid functions
  void refresh_valid_functions();

protected:

  void compute_pts(const zc::DynMathObject<zc_t> &f, zg::SampledCurve&);

  const zg::ZeViewMapper& mapper;

  std::unordered_map<const zc::DynMathObject<zc_t>*, zg::SampledCurve> curves;
  zg::pixel_unit pixelStep;
  zg::Range1D<zg::u<zg::view>> viewRange;
};
