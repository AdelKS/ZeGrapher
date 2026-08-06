// A square button that sits across the right edge of a pane, and toggles
// 'checked' on a click. The pane covers the inner third of it. The icon is a
// child that the user of the button declares.

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

  // a child of the pane with a negative z is drawn under the background of the
  // pane, so the shadow only shows outside the pane
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
