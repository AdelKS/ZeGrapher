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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "themedcolor.h"
#include "Utils/yaml.h"

#include <QGuiApplication>
#include <QStyleHints>

bool isDarkTheme()
{
  return qGuiApp->styleHints()->colorScheme() != Qt::ColorScheme::Light;
}

const QColor& ThemedColor::getCurrent() const
{
  if (isDarkTheme())
    return dark;
  else return light;
}

void ThemedColor::setCurrent(QColor c)
{
  if (isDarkTheme())
    dark = c;
  else
    light = c;
}

std::optional<ThemedColor::POD> ThemedColor::exportPod(ThemedColor defaultColors) const
{
  auto pod = POD {
    .dark = zg::yml::not_default(dark, defaultColors.dark).transform([](QColor c){ return c.name().toStdString(); }),
    .light = zg::yml::not_default(light, defaultColors.light).transform([](QColor c){ return c.name().toStdString(); }),
  };

  if (pod)
    return pod;
  else return {};
}

void ThemedColor::importPod(POD p)
{
  if (p.dark)
    dark = QColor::fromString(QString::fromStdString(*p.dark));

  if (p.light)
    light = QColor::fromString(QString::fromStdString(*p.light));
}
