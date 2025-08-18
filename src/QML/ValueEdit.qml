// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  readonly property int slot: Information.addMathObject();
  readonly property MathObject mathObj: Information.getMathObject(slot);

  property Expr exprBackend: {
    mathObj.setBackend(MathObject.EXPR);
    exprBackend = mathObj.getExpr();
  }

  property double value
  property string implicitName
  property alias exprEdit: zcExprEdit
  property alias expression: zcExprEdit.expression
  property alias customErrorMsg: zcExprEdit.customErrorMsg
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  function refresh() {
    zcExprEdit.refresh();
  }

  ZcExprEdit {
    id: zcExprEdit
    backend: exprBackend

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1
  }

  Component.onCompleted: {

    zcExprEdit.backend = exprBackend;
    value = Qt.binding(function() { return exprBackend.value; });
    exprBackend.implicitName = implicitName;
    implicitName = Qt.binding(function() { return exprBackend.implicitName; });
    console.debug("ValueEdit: backend=", exprBackend);
  }

  Component.onDestruction: {
    Information.removeMathObject(slot);
  }
}
