// One column of a DataSheet: the name it is known by, plus its table toggle

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Data backend

  property alias dragHandle: dragHandle

  signal deleteRequested()

  implicitHeight: frame.implicitHeight + del.height / 2
  implicitWidth: frame.implicitWidth + del.width / 2

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

    ColumnLayout {
      anchors.fill: parent
      spacing: 5

      RowLayout {
        id: layout
        Layout.fillWidth: true
        spacing: 5

        NameEdit {
          backend: root.backend

          Layout.fillWidth: true
          Layout.alignment: Qt.AlignVCenter
        }

        IconRoundButton {
          id: displayButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30
          Layout.alignment: Qt.AlignVCenter

          checkable: true
          checked: !root.backend.style.visible

          iconName: checked ? "closed-eye" : "open-eye"

          ToolTip.text: checked ? qsTr("Show the curve of this column")
                                : qsTr("Hide the curve of this column")

          onToggled: root.backend.style.visible = !checked
        }

        IconRoundButton {
          id: styleButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30
          Layout.alignment: Qt.AlignVCenter

          checkable: true
          checked: false

          iconName: "brush"

          ToolTip.text: qsTr("Plot style")
        }

        IconRoundButton {
          id: sampleButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30
          Layout.alignment: Qt.AlignVCenter

          checkable: true
          checked: false

          iconName: "sample"

          ToolTip.text: qsTr("Fill this column by sampling an object")
        }

        IconRoundButton {
          id: showInTable
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30
          Layout.alignment: Qt.AlignVCenter

          checkable: true
          checked: root.backend.showInTable
          iconName: "table"

          ToolTip.text: qsTr("Show this column in the table")

          onToggled: root.backend.showInTable = checked;
        }

        ColorButton {
          id: colorButton
          radius: 12

          Layout.alignment: Qt.AlignVCenter

          ToolTip.text: qsTr("The color of the curve")

          selectedColor: root.backend.style.color

          onSelectedColorModified: root.backend.style.color = selectedColor
        }

        DragHandle {
          id: dragHandle
          Layout.alignment: Qt.AlignVCenter

          ToolTip.text: qsTr("Drag to change the order of the columns (in the exported CSV and in the plot)")
        }
      }

      ObjectStyle {
        base: root.backend
        style: root.backend.style

        open: styleButton.checked
      }

      SamplingEdit {
        column: root.backend

        open: sampleButton.checked

        onApplied: sampleButton.checked = false
      }
    }
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

    ToolTip.text: qsTr("Delete this column")

    onReleased: root.deleteRequested();
  }
}
