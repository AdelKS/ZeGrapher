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

#pragma once

#include <QColor>
#include <QMetaType>
#include <QtQmlIntegration/qqmlintegration.h>

struct ThemedColor
{
  Q_GADGET
  QML_VALUE_TYPE(themedColor)

  Q_PROPERTY(QColor current READ getCurrent WRITE setCurrent)
  Q_PROPERTY(QColor dark MEMBER dark)
  Q_PROPERTY(QColor light MEMBER light)

public:
  /// @brief color for dark theme
  QColor dark;

  /// @brief color for light theme
  QColor light;

  /// @brief returns the color according to current system theme
  Q_INVOKABLE const QColor& getCurrent() const;

  /// @brief sets the color for the current system theme
  /// @note  the color for the other theme is also updated using HSV
  //         where the value is set to 255 - value of the set color
  Q_INVOKABLE void setCurrent(QColor);

  bool operator == (const ThemedColor&) const = default;
};
