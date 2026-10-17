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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
