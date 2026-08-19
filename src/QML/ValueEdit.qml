// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Controls.FluentWinUI3

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

    // call sites set the hint with ToolTip.text
    hint: root.ToolTip.text

    nanValue: isNaN(root.backend.value)

    expression: root.backend.expression
    onTextEdited: (expression) => root.backend.setExpression(expression)

    anchors.left: parent.left
    anchors.right: parent.right

    highlighter.offset: root.backend.implicitName.length + 1

    state: root.backend.state

  }

  Component.onCompleted: {
    refresh();
  }
}
