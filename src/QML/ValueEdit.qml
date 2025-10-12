// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  required property Expr backend

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

    nanValue: isNaN(root.backend.value)

    onTextEdited: root.backend.setExpression(expression);

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: root.backend.implicitName.length + 1

    state: root.backend.state

    Component.onCompleted: root.backend.setExpression(expression)
  }

  Component.onCompleted: {
    console.debug("ValueEdit: backend=", backend);
    refresh();
  }
}
