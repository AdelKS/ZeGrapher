import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias textInput: edit
  property double value: NaN

  signal numberEdited();

  function setValue(val: double) {
    if (!isNaN(val))
      edit.text = val.toString();
    else edit.text = "";

    value = val;
  }

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
    anchors.leftMargin: 6
    anchors.rightMargin: 6
    color: myPalette.text
    focus: true
    font: Information.appSettings.font
    text: ""
    validator: DoubleValidator{}

    onTextEdited: {
      rec.value = parseFloat(text);
      rec.numberEdited();
    }
  }

  TextMetrics {
    id: textMetrics
    text: "=+-*/_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    font: edit.font
  }

}
