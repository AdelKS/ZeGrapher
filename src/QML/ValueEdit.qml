// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  readonly property MathObject mathObj: MathWorld.addAltMathObject(MathObject.EXPR)

  property Expr backend: mathObj.getExpr()

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
      target: root.backend
      function onStateChanged() {
        zcExprEdit.setState(root.backend.state);
      }
    }

    onTextEdited: root.backend.setExpression(expression);

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: implicitName.length + 1

    Component.onCompleted: root.backend.setExpression(expression)
  }

  Component.onCompleted: {
    value = Qt.binding(function() { return backend.value; });
    backend.implicitName = implicitName;
    implicitName = Qt.binding(function() { return backend.implicitName; });
    console.debug("ValueEdit: backend=", backend);
  }

  Component.onDestruction: {
    MathWorld.removeMathObject(mathObj);
  }
}
