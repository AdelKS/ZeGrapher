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

pragma Singleton

// Values shared by several widgets, which read them from here instead of
// keeping a copy

import QtQuick

QtObject {
  /// @brief how long the pointer must rest on a widget before its hint shows,
  ///        in milliseconds
  readonly property int tooltipDelay: 600

  /// @brief the width of one level of indent in a markdown list, in pixels
  readonly property real listIndent: 14

  /// @brief the space between a vertical scrollbar and the content beside it,
  ///        in pixels
  readonly property int scrollBarGap: 8

  /// @brief the link color, the same as a valid expression's
  readonly property color linkColor: themed(Information.appSettings.validSyntax)

  /// @brief the color of a link under the pointer
  // Qt.lighter on a dark window, and Qt.darker on a light one. One function for
  // both themes washes the link out on one of the two
  readonly property color hoveredLinkColor: dark ? Qt.lighter(linkColor, 1.4)
                                                 : Qt.darker(linkColor, 1.4)

  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }

  /// @brief true when the app uses its dark theme. The FluentWinUI3 style is
  ///        dark whenever the color scheme isn't Light, Unknown included, and
  ///        isDarkTheme() in C++ follows the same rule
  readonly property bool dark: Application.styleHints.colorScheme !== Qt.ColorScheme.Light

  /// @brief a ThemedColor's color in the app's current theme
  // ThemedColor::getCurrent() reads the color scheme in C++, which bindings
  // don't track. This function reads 'dark', so bindings that call it
  // re-evaluate when the scheme changes
  function themed(c: themedColor): color {
    return dark ? c.dark : c.light;
  }

  /// @brief the path of an icon from icons/ in the app's current theme: the
  ///        '-light' file on a dark window, the '-dark' file on a light one
  function icon(name: string): string {
    return "qrc:/icons/" + name + (dark ? "-light" : "-dark") + ".svg";
  }
}
