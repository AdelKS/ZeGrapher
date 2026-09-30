// One column of a DataSheet: the name it is known by, plus its table toggle

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

ObjectCard {
  id: root

  required property Data backend

  property alias dragHandle: dragHandle

  deleteHint: qsTr("Delete this column")

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

      CardButton {
        id: displayButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: !root.backend.style.visible

        iconName: checked ? "closed-eye" : "open-eye"

        ToolTip.text: checked ? qsTr("Show the curve of this column")
                              : qsTr("Hide the curve of this column")

        onToggled: root.backend.style.visible = !checked
      }

      CardButton {
        id: styleButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: false

        iconName: "brush"

        ToolTip.text: qsTr("Plot style")
      }

      CardButton {
        id: sampleButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: false

        iconName: "sample"

        ToolTip.text: qsTr("Fill this column by sampling an object")
      }

      CardButton {
        id: showInTable
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
