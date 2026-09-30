// The frame of an object in a list, with a delete button in its top right
// corner

import QtQuick
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  default property alias content: frame.contentData
  property alias frame: frame

  /// @brief the delete button's hint
  required property string deleteHint

  signal deleteRequested()

  implicitWidth: frame.implicitWidth + del.width / 2
  implicitHeight: frame.implicitHeight + del.height / 2

  Frame {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: del.width / 2
    anchors.top: parent.top
    anchors.topMargin: del.height / 2
    anchors.bottom: parent.bottom

    leftPadding: 10
    rightPadding: 10
    topPadding: 5
    bottomPadding: 5
  }

  IconRoundButton {
    id: del
    width: 15
    height: 15
    anchors.horizontalCenter: frame.right
    anchors.horizontalCenterOffset: -2
    anchors.verticalCenter: frame.top
    anchors.verticalCenterOffset: 2
    z: 1

    iconName: "delete"

    ToolTip.text: root.deleteHint

    onReleased: root.deleteRequested()
  }
}
