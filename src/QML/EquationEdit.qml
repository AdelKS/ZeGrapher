// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  required property MathObject mathObj
  required property PlotStyle style

  property ZcMathObject zcMathObj
  property Equation zcBackend

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

  ZcExprEdit {
    id: zcExprEdit
    mathObj: zcMathObj

    anchors.left: parent.left
    anchors.right: parent.right
  }

  Component.onCompleted: {
    mathObj.style = style;
    mathObj.setBackend(MathObject.ZCMATHOBJECT);
    zcMathObj = mathObj.getZcMathObject();
    zcMathObj.setBackend(ZcMathObject.EQUATION);
    zcBackend = zcMathObj.getEquation();
    console.debug("EquationEdit: backend=", zcBackend);
    console.debug("EquationEdit: style=", style);
  }
}
