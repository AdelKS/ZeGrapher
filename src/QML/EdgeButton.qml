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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// A square button across the right edge of a pane, which toggles 'checked' on
// click. The pane covers its inner third. Users of the button declare the icon
// as a child.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Effects

Rectangle {
  id: root

  property bool checked: false

  /// @brief the width of the part outside the pane
  readonly property int apparentWidth: 2 * width / 3

  width: 25
  height: width
  radius: 8
  color: ZeStyle.palette.window

  anchors.right: parent.right
  anchors.rightMargin: -apparentWidth

  // a pane child with a negative z is drawn under the pane's background, so the
  // shadow only shows outside the pane
  RectangularShadow {
    parent: root.parent
    z: -2
    x: root.x
    y: root.y
    width: root.width
    height: root.height
    radius: root.radius
    blur: 10
    spread: 0
    color: ZeStyle.palette.shadow
  }

  // call sites set the hint with ToolTip.text, which screen readers read too
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: mouseArea.containsMouse && ToolTip.text.length !== 0

  Accessible.role: Accessible.Button
  Accessible.name: ToolTip.text
  Accessible.checkable: true
  Accessible.checked: root.checked
  Accessible.onPressAction: root.checked = !root.checked

  MouseArea {
    id: mouseArea

    anchors.fill: parent
    hoverEnabled: true
    cursorShape: Qt.PointingHandCursor
    onClicked: root.checked = !root.checked
  }
}
