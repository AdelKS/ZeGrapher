// A Loader that unfolds its item to the item's implicit height on demand, and
// folds it back

import QtQuick
import QtQuick.Layouts

Loader {
  id: root

  /// @brief whether the item is unfolded
  property bool open: false

  /// @brief the height currently given to the item, animated
  property real shownHeight: 0

  active: open || shownHeight > 0

  Layout.fillWidth: true
  Layout.preferredHeight: shownHeight

  clip: true

  visible: shownHeight > 0

  Behavior on shownHeight { SmoothedAnimation { duration: 200 } }

  states: [
    State {
      name: "folded"; when: !root.open
      PropertyChanges { root.shownHeight: 0 }
    },
    State {
      name: "unfolded"; when: root.open
      PropertyChanges {
        explicit: false
        root.shownHeight: root.implicitHeight
      }
    }
  ]
}
