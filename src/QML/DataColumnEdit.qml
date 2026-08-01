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

          lightThemeIcon: checked ? "qrc:/icons/closed-eye.svg" : "qrc:/icons/open-eye.svg"
          darkThemeIcon: checked ? "qrc:/icons/closed-eye-light.svg" : "qrc:/icons/open-eye-light.svg"

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

          lightThemeIcon: "qrc:/icons/brush.svg"
          darkThemeIcon: "qrc:/icons/brush-light.svg"
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
          lightThemeIcon: "qrc:/icons/table.svg"
          darkThemeIcon: "qrc:/icons/table-light.svg"

          onToggled: root.backend.showInTable = checked;
        }

        ColorButton {
          id: colorButton
          radius: 12

          Layout.alignment: Qt.AlignVCenter

          selectedColor: root.backend.style.color

          onSelectedColorModified: root.backend.style.color = selectedColor
        }

        Item {
          id: dragHandle
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30
          Layout.alignment: Qt.AlignVCenter

          Image {
            anchors.fill: parent
            source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/drag-handle-dark.svg" : "qrc:/icons/drag-handle-light.svg"
            fillMode: Image.PreserveAspectFit
            mipmap: true
          }
        }
      }

      ObjectStyle {
        base: root.backend
        style: root.backend.style

        open: styleButton.checked
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

    lightThemeIcon: "qrc:/icons/delete-dark.svg"
    darkThemeIcon: "qrc:/icons/delete-light.svg"

    onReleased: root.deleteRequested();
  }
}
