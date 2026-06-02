// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  required property Equation backend

  property alias expression: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  Connections {
    target: backend
    function onEquationChanged() {
      zcExprEdit.expression = backend.equation;
    }
  }

  ZcExprEdit {
    id: zcExprEdit

    state: root.backend.state

    expression: backend.equation
    onTextEdited: backend.setEquation(expression)

    anchors.left: parent.left
    anchors.right: parent.right
  }

  Component.onCompleted: {
    console.debug("backendEdit: backend=", backend);
  }
}
