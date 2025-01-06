import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias textInput: edit
  property int textMargin: 5

  clip: true

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  border.width: 1.5
  radius: 4

  border.color: edit.text === "" ? "grey" : Information.appSettings.validSyntax;

  height: Math.max(textMetrics.height, edit.font.pixelSize) + 2*rec.textMargin + border.width


  color: myPalette.base

  TextInput {
    id: edit
    verticalAlignment: TextInput.AlignVCenter
    anchors.fill: parent
    anchors.margins: rec.textMargin
    color: myPalette.text
    focus: true
    text: ""
    validator: DoubleValidator{}
  }

  TextMetrics {
    id: textMetrics
    text: edit.text
  }

}
