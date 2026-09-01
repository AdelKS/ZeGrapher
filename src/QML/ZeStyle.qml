pragma Singleton

// The values that several widgets share. A widget reads a value from here
// instead of holding a copy of it

import QtQuick

QtObject {
  /// @brief how long the pointer must rest on a widget before its hint shows,
  ///        in milliseconds
  readonly property int tooltipDelay: 600

  /// @brief the width of one level of indent in a markdown list, in pixels
  readonly property real listIndent: 14

  /// @brief the thickness of a line that separates two blocks, in pixels
  readonly property real separatorThickness: 3

  /// @brief the color of a link, the same color as a valid expression
  readonly property color linkColor: Information.appSettings.validSyntax.current

  /// @brief the color of a link under the pointer
  // Qt.darker on a light window, and Qt.lighter on a dark one. One function for
  // both themes washes the link out on one of the two
  readonly property color hoveredLinkColor: palette.window.hslLightness > 0.5 ?
                                              Qt.darker(linkColor, 1.4) : Qt.lighter(linkColor, 1.4)

  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }
}
