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

// A list whose rows the user reorders by dragging their handle. Each row is a
// ListViewItem, which folds away before its removal

import QtQuick

ListView {
  signal moveRequested(int from, int to)

  /// @brief a row folded away, 'modelData' being its entry in the model
  signal removeRequested(var modelData)

  move: Transition {
    NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
  }
  moveDisplaced: Transition {
    NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
  }
}
