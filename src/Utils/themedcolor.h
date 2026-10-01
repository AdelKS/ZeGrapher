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

/// @brief true when the app uses its dark theme
/// @note the FluentWinUI3 style is dark whenever QStyleHints::colorScheme()
///       isn't Light, and so is the app: an Unknown scheme counts as dark
bool isDarkTheme();

struct ThemedColor
{
  Q_GADGET
  QML_VALUE_TYPE(themedColor)

  Q_PROPERTY(QColor dark MEMBER dark)
  Q_PROPERTY(QColor light MEMBER light)

public:
  /// @brief color for dark theme
  QColor dark;

  /// @brief color for light theme
  QColor light;

  /// @brief returns the color for the current system theme
  /// @note QML reads a ThemedColor through ZeStyle.themed(), which follows
  ///       color scheme changes
  const QColor& getCurrent() const;

  /// @brief sets the color for the current system theme
  Q_INVOKABLE void setCurrent(QColor);

  bool operator == (const ThemedColor&) const = default;

  struct POD {
    std::optional<std::string> dark;
    std::optional<std::string> light;
    operator bool () const { return dark or light; }
  };

  std::optional<POD> exportPod(ThemedColor defaultColors) const;
  void importPod(POD);
};
