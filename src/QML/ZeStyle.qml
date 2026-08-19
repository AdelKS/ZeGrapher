pragma Singleton

// The values that widgets share, in place of each widget holding its own copy

import QtQuick

QtObject {
  /// @brief how long the pointer rests on a widget before its hint shows, in ms
  readonly property int tooltipDelay: 600
}
