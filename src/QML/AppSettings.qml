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

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Syntax coloring')
      }
      Frame {
        GridLayout {
          columns: 2

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Valid")
          }
          ColorButton {
            id: validSyntaxColorButton
            Layout.alignment: Qt.AlignHCenter

            selectedColor: { selectedColor = Information.appSettings.validSyntax; }

            onSelectedColorChanged: Information.appSettings.validSyntax = selectedColor;

            Connections {
              target: Information.appSettings

              function onWarningSyntaxChanged() {
                if (validSyntaxColorButton.selectedColor !== Information.appSettings.validSyntax) {
                  validSyntaxColorButton.selectedColor = Information.appSettings.validSyntax
                }
              }
            }
          }

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Warning")
          }
          ColorButton {
            id: warningSyntaxColorButton
            Layout.alignment: Qt.AlignHCenter

            selectedColor: { selectedColor = Information.appSettings.warningSyntax; }

            onSelectedColorChanged: Information.appSettings.warningSyntax = selectedColor;

            Connections {
              target: Information.appSettings

              function onWarningSyntaxChanged() {
                if (warningSyntaxColorButton.selectedColor !== Information.appSettings.warningSyntax) {
                  warningSyntaxColorButton.selectedColor = Information.appSettings.warningSyntax
                }
              }
            }
          }

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Invalid")
          }
          ColorButton {
            id: invalidSyntaxColorButton
            Layout.alignment: Qt.AlignHCenter

            selectedColor: { selectedColor = Information.appSettings.invalidSyntax; }

            onSelectedColorChanged: Information.appSettings.invalidSyntax = selectedColor;

            Connections {
              target: Information.appSettings

              function onWarningSyntaxChanged() {
                if (invalidSyntaxColorButton.selectedColor !== Information.appSettings.invalidSyntax) {
                  invalidSyntaxColorButton.selectedColor = Information.appSettings.invalidSyntax
                }
              }
            }
          }
        }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Check for updates')
      }

      IconButton {
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumHeight: 25
        Layout.maximumWidth: 25

        lightThemeIcon: 'qrc:/icons/loop.svg'
        darkThemeIcon: 'qrc:/icons/loop-light.svg'

        onReleased: checker.refresh()
      }

      Label {
        id: updateCheckText
        property int m_height: 0

        Layout.alignment: Qt.AlignHCenter
        Layout.columnSpan: 2
        Layout.maximumHeight: m_height
        Layout.maximumWidth: 200

        wrapMode: Text.WordWrap

        Behavior on m_height { SmoothedAnimation { duration: 200 } }


        UpdateCheck {
          id: checker

          onStatusChanged: {
            switch(status) {
              case UpdateCheck.IDLE:
                updateCheckText.text = "";
                break;
              case UpdateCheck.CHECKING:
                updateCheckText.text = qsTr("Checking...");
                break;
              case UpdateCheck.ERROR:
                updateCheckText.text = qsTr("An error occurred.");
                break;
              case UpdateCheck.UPDATE_MAYBE_AVAILABLE:
                updateCheckText.text = qsTr("Update maybe available, visit zegrapher.com to download it.");
                break;
              case UpdateCheck.UPDATE_AVAILABLE:
                updateCheckText.text = qsTr("Update available, visit zegrapher.com to download it.");
                break;
              case UpdateCheck.UP_TO_DATE:
                updateCheckText.text = qsTr("You have the latest version.");

                break;
              default:
                updateCheckText.text = qsTr("Unhandled error, please report this issue.");
                break;
            }

            updateCheckText.text = updateCheckText.text + "\n" + qsTr("Current version:") + " " + checker.currentVersion;

            if ([UpdateCheck.UPDATE_MAYBE_AVAILABLE, UpdateCheck.UPDATE_AVAILABLE, UpdateCheck.UP_TO_DATE].includes(status)) {
              updateCheckText.text = updateCheckText.text + "\n" + qsTr("Latest stable release:") + " " + checker.latestVersion;
            }
            if (status !== UpdateCheck.IDLE ) {
              updateCheckText.m_height = updateCheckText.implicitHeight;
            } else {
              updateCheckText.m_height = 0;
            }

          }
        }

      }
    }

  }



}
