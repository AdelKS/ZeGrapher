// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  FontDialog {
    id: fontDialog
    currentFont: Information.appFont

    onAccepted: Information.setAppFont(selectedFont)
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((availableWidth - grid.implicitWidth)/2, 0)

    GridLayout {
      id: grid
      anchors.centerIn: parent

      columns: 2
      columnSpacing: 5

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Language')
      }
      ComboBox {
        Layout.alignment: Qt.AlignHCenter

        implicitContentWidthPolicy: ComboBox.WidestText
        background.implicitWidth: implicitContentWidth
        textRole: "text"

        model: ListModel {
          ListElement { text: "English" }
        }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Font')
      }
      Button {
        Layout.alignment: Qt.AlignHCenter
        text: fontDialog.currentFont.family
        onClicked: fontDialog.open()
      }
    }

  }



}
