// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  readonly property int slot: Information.addMathObject();
  readonly property MathObject mathObj: Information.getMathObject(slot);

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
    mathObj: zcMathObj

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1
  }

  Component.onCompleted: {
    mathObj.setBackend(MathObject.ZCMATHOBJECT);
    let zcMathObj = mathObj.getZcMathObject();
    zcMathObj.setBackend(ZcMathObject.EXPR);
    zcExprEdit.mathObj = zcMathObj;

    let exprBackend = zcMathObj.getExpr();
    value = Qt.binding(function() { return exprBackend.value; });
    exprBackend.implicitName = implicitName;
    implicitName = Qt.binding(function() { return exprBackend.implicitName; });
    console.debug("ValueEdit: backend=", exprBackend);
  }

  Component.onDestruction: {
    Information.removeMathObject(slot);
  }
}
