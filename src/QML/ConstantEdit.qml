// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  property alias name: zcExprEdit.expression
  property alias backend: zcBackend
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  Constant {
    id: zcBackend
    name: zcExprEdit.expression
    state: zcExprEdit.state
  }

  RowLayout {
    anchors.fill: parent

    ZcExprEdit {
      id: zcExprEdit

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter
    }

    Label {
      text: "="
      Layout.maximumWidth: 20
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
    }

    NumberEdit
    {
      id: constant

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter

      focus: true
      textInput.validator: DoubleValidator{}

      Connections {
        target: constant.textInput
        function onTextEdited() {
          console.log("new double value: ", constant.textInput.text);
          zcBackend.set_value(parseFloat(constant.textInput.text));
        }
      }
    }

  }


}
