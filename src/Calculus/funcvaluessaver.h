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

#include "GraphDraw/viewmapper.h"
#include "information.h"

#include <unordered_set>

struct FuncCurve
{
  const zc::Function<zc_t>& func;
  const size_t slot;
  std::string equation;
  std::vector<zg::real_pt> curve = {};
  std::unordered_set<size_t> discontinuities = {};
};

class FuncValuesSaver
{
public:
  FuncValuesSaver(const zg::ZeViewMapper& mapper, double pxStep);

  void setPixelStep(double pxStep);
  void update();

  const zc::SlottedDeque<FuncCurve>& getFunCurves() const { return funCurves; }

protected:

  enum Side {LEFT, RIGHT};

  /// @brief refreshes 'funcCurves' for the currently valid functions
  void refresh_valid_functions();

  /// @brief check if equation of functions changed
  void check_for_equation_changes();

  /// @brief removes points that are not within the view, i.e. invisible
  void clear_hidden_pts();

  /// @brief computes points on uniformly distributed view abscissas
  void compute_uniform_visible_pts(size_t slot);

  /// @brief computes more points where the function variation is too steep
  void refine_visible_pts(size_t slot);

  /// @brief computes more points where the function variation is too steep
  void find_discontinuities(size_t slot);

  const zg::ZeViewMapper& mapper;

  zc::SlottedDeque<FuncCurve> funCurves;
  zg::pixel_unit pixelStep;
  size_t pxStepMaxDivider = 32;
  zg::Range1D<zg::u<zg::view>> viewRange;
};
