// A line edit where the user enters an expression that evaluates to a value

import QtQuick

Item {
  id: root

  property alias expression: zcExprEdit.expression
  property alias backend: zcBackend
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Equation {
    id: zcBackend
    equation: zcExprEdit.expression
    state: zcExprEdit.state
  }

  ZcExprEdit {
    id: zcExprEdit

    anchors.left: parent.left
    anchors.right: parent.right
  }
}
