/****************************************************************************
**  Copyright (c) 2022, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "structures.h"
#include "Utils/yaml.h"

bool ptCompY(const Point &pt1, const Point &pt2)
{
  return pt1.y < pt2.y;
}

Point operator * (const Point &pt1, const Point &pt2)
{
  return Point {.x = pt1.x * pt2.x, .y = pt1.y * pt2.y };
}

std::optional<ZeSizeSettings::POD> ZeSizeSettings::exportPod() const
{
  ZeSizeSettings::POD p;

  p.scaling = zg::yml::not_default(scalingFactor, 1.0);

  if (not sheetFillsWindow)
  {
    p.width = cmSheetSize.width();
    p.height = cmSheetSize.height();
  }

  return zg::yml::if_any(p);
}

void ZeSizeSettings::importPod(POD p)
{
  if (p.scaling)
    scalingFactor = zg::yml::clamped(*p.scaling, minScaling, maxScaling);

  // a sheet without a positive size fills the window
  if (p.width.value_or(0) > 0 and p.height.value_or(0) > 0)
  {
    sheetFillsWindow = false;
    cmSheetSize = QSizeF(zg::yml::clamped(*p.width, minCm, maxCm),
                         zg::yml::clamped(*p.height, minCm, maxCm));
  }
}
