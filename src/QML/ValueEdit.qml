// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  readonly property MathObject mathObj: MathWorld.addAltMathObject(MathObject.EXPR)

  property Expr exprBackend: mathObj.getExpr()

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

    Connections {
      target: root.exprBackend
      function onStateChanged() {
        zcExprEdit.setState(root.exprBackend.state);
      }
    }

    onExpressionChanged: root.exprBackend.setExpression(expression);

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1
  }

  Component.onCompleted: {
    value = Qt.binding(function() { return exprBackend.value; });
    exprBackend.implicitName = implicitName;
    implicitName = Qt.binding(function() { return exprBackend.implicitName; });
    console.debug("ValueEdit: backend=", exprBackend);
  }

  Component.onDestruction: {
    MathWorld.removeMathObject(mathObj);
  }
}
