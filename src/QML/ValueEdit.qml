// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import zegrapher.highlighter 1.0
import zegrapher.information 1.0
import zegrapher.expr 1.0
import zegrapher.opterror 1.0
import zegrapher.mathobject 1.0

Item {
  id: root

  readonly property alias value: exprBackend.value;
  property alias implicitName : exprBackend.implicitName;
  property alias expression: zcExprEdit.expression
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  function setCustomErrorMsg(msg: string) {
    zcExprEdit.setCustomErrorMsg(msg);
  }

  MathObject {
    id: mathObj
  }

  Expr {
    id: exprBackend
    expression: zcExprEdit.expression
    Component.onCompleted: mathObj.setBackend(exprBackend);
  }

  ZcExprEdit {
    id: zcExprEdit

    anchors.left: parent.left
    anchors.right: parent.right

    optError: exprBackend.optError
    highlighter.offset: implicitName.length+1
  }
}
