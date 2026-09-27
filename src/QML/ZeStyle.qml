pragma Singleton

// The values that several widgets share. A widget reads a value from here
// instead of holding a copy of it

import QtQuick

QtObject {
  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }

  /// @brief true when the app draws its dark theme. The FluentWinUI3 style
  ///        draws dark when the color scheme is not Light, an Unknown scheme
  ///        included, and isDarkTheme() in C++ follows the same rule
  readonly property bool dark: Application.styleHints.colorScheme !== Qt.ColorScheme.Light
}
