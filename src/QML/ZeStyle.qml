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

  /// @brief a ThemedColor's color in the app's current theme
  // ThemedColor::getCurrent() reads the color scheme in C++, which bindings
  // don't track. This function reads 'dark', so bindings that call it
  // re-evaluate when the scheme changes
  function themed(c: themedColor): color {
    return dark ? c.dark : c.light;
  }
}
