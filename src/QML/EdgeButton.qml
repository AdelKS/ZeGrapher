// A square button across the right edge of a pane, which toggles 'checked' on
// click. The pane covers its inner third. Users of the button declare the icon
// as a child.

import QtQuick
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

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor
    onClicked: root.checked = !root.checked
  }
}
