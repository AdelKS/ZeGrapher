import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias textInput: edit
  property double value: NaN

  // Two-way bindable: bind `value` to the source and push edits back via the
  // signal argument, so the binding survives user edits:
  //   value: backend.foo
  //   onNumberEdited: (value) => backend.foo = value
  signal numberEdited(real value);

  // mirror external/programmatic value changes into the field, but leave the
  // text alone while the user is mid-edit (e.g. typing "1." parses to 1)
  onValueChanged: {
    if (parseFloat(edit.text) !== value)
      edit.text = isNaN(value) ? "" : value.toString();
  }
  Component.onCompleted: edit.text = isNaN(value) ? "" : value.toString();

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

    onTextEdited: rec.numberEdited(parseFloat(text));
  }

  TextMetrics {
    id: textMetrics
    text: "=+-*/_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    font: edit.font
  }

}
