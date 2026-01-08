import QtQuick
import QtQuick.Controls.FluentWinUI3

Item {
  id: root
  property alias suffix: spinBox.suffix
  property alias prefix: spinBox.prefix
  property alias decimals: spinBox.decimals

  signal valueModified();

  property real from: 0.0
  property real value: 1.0
  property real to: 1.0
  property real step: 0.1

  onValueChanged: spinBox.set_value(value);

  implicitHeight: spinBox.implicitHeight
  implicitWidth: spinBox.implicitWidth

  SpinBox {
    id: spinBox
    anchors.fill: parent

    property string suffix: ''
    property string prefix: ''
    property int decimals: 1
    readonly property int decimalFactor: Math.pow(10, decimals)

    function decimalToInt(decimal) {
      return decimal * decimalFactor
    }

    function set_value(val: real) {
      value = decimalToInt(val);
    }

    onValueModified: {
      root.value = value / decimalFactor;
      root.valueModified();
    }

    from: decimalToInt(root.from)
    value: { value = decimalToInt(root.value); }
    to: decimalToInt(root.to)
    stepSize: decimalToInt(root.step)
    editable: true

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
