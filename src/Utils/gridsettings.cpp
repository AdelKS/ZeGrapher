/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "gridsettings.h"
#include "yaml.h"

std::optional<Ze1DSubgridSettings::POD> Ze1DSubgridSettings::exportPod() const
{
  return zg::yml::if_any(POD {
    .show = zg::yml::not_default(show, defaultShow),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWidth),
    .color = color.exportPod(defaultColor),
    .subdivs = zg::yml::not_default(subdivs, defaultSubdivs)
  });
}

void Ze1DSubgridSettings::importPod(POD p)
{
  if (p.show)
    show = *p.show;

  if (p.line_width)
    lineWidth = zg::yml::clamped(*p.line_width, minLineWidth, maxLineWidth);

  if (p.color)
    color.importPod(std::move(*p.color));

  if (p.subdivs)
    subdivs = zg::yml::clamped(*p.subdivs, minSubdivs, maxSubdivs);
}

std::optional<Ze1DGridSettings::POD> Ze1DGridSettings::exportPod() const
{
  return zg::yml::if_any(POD {
    .show = zg::yml::not_default(show, defaultShow),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWidth),
    .color = color.exportPod(defaultColor),
  });
}

void Ze1DGridSettings::importPod(POD p)
{
  if (p.show)
    show = *p.show;

  if (p.line_width)
    lineWidth = zg::yml::clamped(*p.line_width, minLineWidth, maxLineWidth);

  if (p.color)
    color.importPod(std::move(*p.color));
}

std::optional<ZeGridSettings::POD> ZeGridSettings::exportPod() const
{
  return zg::yml::if_any(POD {
    .x = x.exportPod(),
    .y = y.exportPod()
  });
}

void ZeGridSettings::importPod(POD p)
{
  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}


std::optional<ZeSubgridSettings::POD> ZeSubgridSettings::exportPod() const
{
  return zg::yml::if_any(POD {
    .x = x.exportPod(),
    .y = y.exportPod()
  });
}

void ZeSubgridSettings::importPod(POD p)
{
  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}
