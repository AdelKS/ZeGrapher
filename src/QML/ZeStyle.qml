pragma Singleton

// Values shared by several widgets, which read them from here instead of
// keeping a copy

import QtQuick

QtObject {
  readonly property SystemPalette palette: SystemPalette { colorGroup: SystemPalette.Active }

  /// @brief true when the app uses its dark theme. The FluentWinUI3 style is
  ///        dark whenever the color scheme isn't Light, Unknown included, and
  ///        isDarkTheme() in C++ follows the same rule
  readonly property bool dark: Application.styleHints.colorScheme !== Qt.ColorScheme.Light
}
