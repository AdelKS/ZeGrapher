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

struct FuncCurve
{
  const zc::Function<zc_t>& func;
  std::vector<std::vector<zg::real_pt>> slices = {};

};

class FuncValuesSaver
{
public:
  FuncValuesSaver(const zg::ZeViewMapper& mapper, double pxStep);

  void setPixelStep(double pxStep);
  void calculateAll();
  void move();

  const std::vector<FuncCurve>& getFunCurves() const { return funCurves; }

protected:
  void calculateAllFuncColors();

  const zg::ZeViewMapper& mapper;

  std::vector<FuncCurve> funCurves;
  zg::pixel_unit pixelStep;
};
