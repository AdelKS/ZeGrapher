// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import zegrapher.highlighter 1.0
import zegrapher.information 1.0
import zegrapher.expr 1.0
import zegrapher.state 1.0
import zegrapher.mathobject 1.0

Item {
  id: root

  readonly property alias value: exprBackend.value;
  property alias implicitName : exprBackend.implicitName;
  property alias state: zcExprEdit.state
  property alias expression: zcExprEdit.expression
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  MathObject {
    id: mathObj
  }

  Expr {
    id: exprBackend
  }

  ZcExprEdit {
    id: zcExprEdit

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1
  }

  Component.onCompleted: {
    mathObj.setBackend(exprBackend);

    exprBackend.expression = Qt.binding(function() { return zcExprEdit.expression })
    exprBackend.state = Qt.binding(function() { return zcExprEdit.state })
  }
}
