pragma Singleton

// The values that widgets share, in place of each widget holding its own copy

import QtQuick

QtObject {
  /// @brief how long the pointer rests on a widget before its hint shows, in ms
  readonly property int tooltipDelay: 600

  /// @brief the step that one level of a markdown list indents by, in pixels
  readonly property real listIndent: 14

  /// @brief how thick a line that separates two blocks is, in pixels
  readonly property real separatorThickness: 3

  /// @brief the color of a link, which is the color of a valid expression
  readonly property color linkColor: Information.appSettings.validSyntax.current

  /// @brief the color of a link under the pointer
  // Qt.lighter on a dark window and Qt.darker on a light one: one of the two
  // washes the link out on the other theme
  readonly property color hoveredLinkColor: palette.window.hslLightness > 0.5 ?
                                              Qt.darker(linkColor, 1.4) : Qt.lighter(linkColor, 1.4)

  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }
}
