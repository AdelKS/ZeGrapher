// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Constant backend

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  RowLayout {
    anchors.fill: parent

    ZcExprEdit {
      id: zcExprEdit

      state: root.backend.state
      onTextEdited: root.backend.setName(expression)

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter
    }

    ZeLabel {
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
          console.debug("new double value: ", constant.textInput.text);
          backend.set_value(parseFloat(constant.textInput.text));
        }
      }
    }
  }

  Component.onCompleted: {
    console.debug("backendEdit: backend=", backend);
  }

}
