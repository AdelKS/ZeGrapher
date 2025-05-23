// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  required property MathObject mathObj

  property alias expression: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  property alias backend: zcBackend
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  Equation {
    id: zcBackend
  }

  ZcExprEdit {
    id: zcExprEdit
    mathObj: root.mathObj

    anchors.left: parent.left
    anchors.right: parent.right
  }

  Component.onCompleted: {
    root.mathObj.setBackend(zcBackend);
  }
}
