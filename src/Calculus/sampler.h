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
#include "MathObjects/mathobject.h"
#include "Utils/sampledcurve.h"

#include <zecalculator/zecalculator.h>

namespace zg {
  struct PlotStyle;
}

class Sampler
{
public:
  Sampler(const zg::ZeViewMapper& mapper, double pxStep);

  void setPixelStep(double pxStep);
  void update();

  const auto& getCurves() { return curves_list; }

  /// @brief clears the saved points of functions whose name is in 'objectNames'
  void clearCache(QStringList objectNames);

  /// @brief refreshes 'curves' for the currently valid objects
  void refresh_valid_objects();

  /// @brief refreshes the style of the currently valid objects
  void refresh_curve_styles();

  /// @brief refreshes the sampling settings of every curve: clears the data if it changed
  void refresh_curve_settings();

  /// @brief makes sure the full [start, step, end] range of values of the schrodinger constant
  ///        is in the keys of the schrodinger_curves_map, no more no less
  void refresh_schrodinger_keys();

  void refresh_curves_list();

protected:

  /// @brief samples an object given the current range and graph size
  /// @tparam continuous: whether the object is continuous or discrete
  template <zg::PlotStyle::CoordinateSystem coordinates>
  void sample(auto, zg::SampledCurve&);

  /// @brief detects and records discontinuities in a fully sampled continuous curve
  void update_discontinuities(zg::SampledCurve& data);

  const zg::ZeViewMapper& mapper;

  std::unordered_map<const zg::MathObject*, zg::SampledCurve> curves;
  std::unordered_map<double, std::unordered_map<const zg::MathObject*, zg::SampledCurve>> schrodinger_curves_map;

  std::vector<std::reference_wrapper<zg::SampledCurve>> curves_list;

  zg::pixel_unit pixelStep;
  zg::Range1D<zg::u<zg::view>> viewRange;
};
