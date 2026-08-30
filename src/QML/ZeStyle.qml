pragma Singleton

// The values that several widgets share. A widget reads a value from here
// instead of holding a copy of it

import QtQuick

QtObject {
  /// @brief the width of one level of indent in a markdown list, in pixels
  readonly property real listIndent: 14

  /// @brief the color of a link, the same color as a valid expression
  readonly property color linkColor: Information.appSettings.validSyntax.current

  /// @brief the color of a link under the pointer
  // Qt.lighter on a dark window, and Qt.darker on a light one. One function for
  // both themes washes the link out on one of the two
  readonly property color hoveredLinkColor: dark ? Qt.lighter(linkColor, 1.4)
                                                 : Qt.darker(linkColor, 1.4)

  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }

  /// @brief true when the app draws its dark theme. The FluentWinUI3 style
  ///        draws dark when the color scheme is not Light, an Unknown scheme
  ///        included, and isDarkTheme() in C++ follows the same rule
  readonly property bool dark: Application.styleHints.colorScheme !== Qt.ColorScheme.Light
}
