// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  readonly property alias value: exprBackend.value;
  property alias implicitName : exprBackend.implicitName;
  property alias state: exprBackend.state
  property alias expression: zcExprEdit.expression
  property alias customErrorMsg: zcExprEdit.customErrorMsg
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  function refresh() {
    zcExprEdit.refresh();
  }

  Expr {
    id: exprBackend
  }

  ZcMathObject {
    id: zcMathObj
  }

  MathObject {
    id: mathObj
  }

  ZcExprEdit {
    id: zcExprEdit
    mathObj: zcMathObj

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1
  }

  Component.onCompleted: {
    console.log("ValueEdit: backend=", exprBackend);
    zcMathObj.setBackend(exprBackend);
    mathObj.setBackend(zcMathObj);
  }
}
