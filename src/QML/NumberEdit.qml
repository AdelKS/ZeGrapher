import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias textInput: edit

  clip: true

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  border.width: 1.5
  radius: 4

  border.color: edit.text === "" ? "grey" : Information.appSettings.validSyntax.current;

  implicitHeight: Math.max(textMetrics.height, edit.font.pixelSize) + 10 + 2*border.width

  color: myPalette.base

  TextInput {
    id: edit
    verticalAlignment: TextInput.AlignVCenter
    anchors.fill: parent
    anchors.leftMargin: 4
    anchors.rightMargin: 4
    color: myPalette.text
    focus: true
    font: Information.appSettings.font
    text: ""
    validator: DoubleValidator{}
  }

  TextMetrics {
    id: textMetrics
    text: "=+-*/_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    font: edit.font
  }

}
