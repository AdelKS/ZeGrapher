/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "axissettings.h"
#include "Utils/yaml.h"

std::optional<ZeAxisSettings::POD> ZeAxisSettings::exportPod(const QString& multiplier) const
{
  return zg::yml::if_any(POD {
    .title = zg::yml::not_default(title),
    .tick_spacing = zg::yml::not_default(tickRelSpacing, 0),
    .max_digits = zg::yml::not_default(linSettings.maxDigitsNum, ZeLinAxisSettings::defaultDigits),
    .multiplier = zg::yml::not_default(multiplier),
  });
}

void ZeAxisSettings::importPod(ZeAxisSettings::POD p)
{
  if (p.tick_spacing)
    tickRelSpacing = zg::yml::clamped(*p.tick_spacing, 0, maxTickSpacing);

  if (p.max_digits)
    linSettings.maxDigitsNum = zg::yml::clamped(*p.max_digits, minDigits, maxDigits);

  if (p.title)
    title = QString::fromStdString(*p.title);
}

std::optional<ZeAxesSettings::POD> ZeAxesSettings::exportPod(const QString& xMultiplier,
                                                            const QString& yMultiplier) const
{
  return zg::yml::if_any(POD {
    .color = color.exportPod(defaultColor),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWidth),
    .x = x.exportPod(xMultiplier),
    .y = y.exportPod(yMultiplier),
    .title_font = zg::yml::QFontPOD::from(titleFont, zg::mathFont())
  });
}

void ZeAxesSettings::importPod(ZeAxesSettings::POD p)
{
  if (p.color)
    color.importPod(*p.color);

  if (p.line_width)
    lineWidth = zg::yml::clamped(*p.line_width, minLineWidth, maxLineWidth);

  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));

  if (p.title_font)
    p.title_font->applyTo(titleFont);
}
