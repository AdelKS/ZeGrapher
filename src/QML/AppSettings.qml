// The settings of the app itself, under the App tab of the panel

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property size globalMenuSize

  FontDialog {
    id: fontDialog
    currentFont: Information.appSettings.font

    onAccepted: {
      Information.appSettings.font = selectedFont;
    }
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - grid.implicitWidth)/2, 0)

    contentHeight: grid.implicitHeight + root.globalMenuSize.height

    GridLayout {
      id: grid
      anchors.centerIn: parent

      columns: 2
      columnSpacing: 10

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Language')
      }
      ComboBox {
        id: langBox

        Layout.alignment: Qt.AlignLeft
        Layout.fillWidth: true

        implicitContentWidthPolicy: ComboBox.WidestText
        background.implicitWidth: implicitContentWidth
        textRole: "text"
        valueRole: "value"

        // one entry per translation the build embedded
        model: Information.appSettings.languages

        currentIndex: langBox.model.findIndex((lang) => lang.value === Information.appSettings.language)

        onActivated: (index) => {
          Information.appSettings.language = langBox.model[index].value;
        }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Font')
      }
      Button {
        Layout.alignment: Qt.AlignLeft
        Layout.fillWidth: true

        text: fontDialog.currentFont.family
        onClicked: fontDialog.open()
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Syntax coloring')
        tooltipText: qsTr("Border color of an expression, one color per state.")
      }
      Frame {
        Layout.fillWidth: true
        GridLayout {
          columns: 2

          ZeLabel {
            id: validSyntaxLabel
            Layout.alignment: Qt.AlignRight
            text: qsTr("Valid")
            tooltipText: qsTr("Border color of a valid expression.")
          }
          ColorButton {
            id: validSyntaxColorButton
            Layout.alignment: Qt.AlignLeft

            ToolTip.text: validSyntaxLabel.tooltipText

            selectedColor: Information.appSettings.validSyntax

            onSelectedColorModified: Information.appSettings.validSyntax = selectedColor;
          }

          ZeLabel {
            id: warningSyntaxLabel
            Layout.alignment: Qt.AlignRight
            text: qsTr("Warning")
            tooltipText: qsTr("Border color of an expression that is valid but has no value.")
          }
          ColorButton {
            id: warningSyntaxColorButton
            Layout.alignment: Qt.AlignLeft

            ToolTip.text: warningSyntaxLabel.tooltipText

            selectedColor: Information.appSettings.warningSyntax

            onSelectedColorModified: Information.appSettings.warningSyntax = selectedColor;
          }

          ZeLabel {
            id: invalidSyntaxLabel
            Layout.alignment: Qt.AlignRight
            text: qsTr("Invalid")
            tooltipText: qsTr("Border color of an invalid expression.")
          }
          ColorButton {
            id: invalidSyntaxColorButton
            Layout.alignment: Qt.AlignLeft

            ToolTip.text: invalidSyntaxLabel.tooltipText

            selectedColor: Information.appSettings.invalidSyntax

            onSelectedColorModified: Information.appSettings.invalidSyntax = selectedColor;
          }
        }
      }
    }

  }



}
