// One column of a DataSheet: the name it is known by, plus its table toggle

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Data backend

  property alias name: zcExprEdit.expression
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

    RowLayout {
      id: layout
      anchors.fill: parent
      spacing: 5

      ZcExprEdit {
        id: zcExprEdit

        state: root.backend.state

        expression: root.backend.name
        onTextEdited: (expression) => root.backend.setName(expression)

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
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
