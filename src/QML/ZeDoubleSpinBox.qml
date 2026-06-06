import QtQuick
import QtQuick.Controls.FluentWinUI3

Item {
  id: root
  property alias suffix: spinBox.suffix
  property alias prefix: spinBox.prefix
  property alias decimals: spinBox.decimals

  property real from: 0.0
  property real to: 1.0
  property real step: 0.1

  // Two-way bindable: bind `value` to the source and push edits back via the
  // signal argument, so the binding survives user edits:
  //   value: backend.foo
  //   onValueModified: (value) => backend.foo = value
  property real value: 0

  signal valueModified(real value)

  onValueChanged: spinBox.set_value(value)
  Component.onCompleted: spinBox.set_value(value)

  function setValue(val: real) {
    value = val;
  }

  function increase() {
    spinBox.increase();
    root.valueModified(spinBox.value / spinBox.decimalFactor);
  }

  function decrease() {
    spinBox.decrease();
    root.valueModified(spinBox.value / spinBox.decimalFactor);
  }

  implicitHeight: spinBox.implicitHeight
  implicitWidth: spinBox.implicitWidth

  SpinBox {
    id: spinBox
    anchors.fill: parent
    font: Information.appSettings.font

    property string suffix: ''
    property string prefix: ''
    property int decimals: 1
    readonly property int decimalFactor: Math.pow(10, decimals)

    function decimalToInt(decimal) {
      return Math.round(decimal * decimalFactor)
    }

    function set_value(val: real) {
      let scaled = decimalToInt(val);
      if (value !== scaled)
        value = scaled;
    }

    from: decimalToInt(root.from)
    to: decimalToInt(root.to)
    stepSize: decimalToInt(root.step)
    editable: true

    // re-assert the value when the range changes, so a widened bound is not
    // left clamped to a stale one (binding evaluation order is not guaranteed)
    onFromChanged: set_value(root.value)
    onToChanged: set_value(root.value)

    onValueModified: root.valueModified(value / decimalFactor)

    readonly property regexp numberExtractionRegExp: /\*?(\d+(\.\d+)?)\*?/

    validator: DoubleValidator {
      bottom: Math.min(root.from, root.to)
      top:  Math.max(root.from, root.to)
      decimals: spinBox.decimals
      notation: DoubleValidator.StandardNotation
    }

    textFromValue: function(value, locale) {
      return prefix + Number(value / decimalFactor).toLocaleString(locale, 'f', spinBox.decimals) + suffix
    }

    valueFromText: function(text, locale) {
      return Math.round(Number.fromLocaleString(locale, numberExtractionRegExp.exec(text)[1]) * decimalFactor)
    }
  }
}
