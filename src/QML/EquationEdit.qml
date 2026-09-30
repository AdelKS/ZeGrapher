// One equation, as one line edit that holds 'f(x) = ...'

import QtQuick

Item {
  id: root

  required property Equation backend

  implicitHeight: zcExprEdit.implicitHeight

  ZcExprEdit {
    id: zcExprEdit

    state: root.backend.state

    expression: root.backend.equation
    onTextEdited: (expression) => root.backend.setEquation(expression)

    anchors.left: parent.left
    anchors.right: parent.right
  }
}
