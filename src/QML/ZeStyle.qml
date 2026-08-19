pragma Singleton

// The values that several widgets share. A widget reads a value from here
// instead of holding a copy of it

import QtQuick

QtObject {
  /// @brief how long the pointer must rest on a widget before its hint shows,
  ///        in milliseconds
  readonly property int tooltipDelay: 600
}
