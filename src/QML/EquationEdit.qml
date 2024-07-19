// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import zegrapher.highlighter 1.0
import zegrapher.information 1.0
import zegrapher.zc 1.0
import zegrapher.opterror 1.0

Item {
  id: root

  property alias expression: zcExprEdit.expression
  property alias type: zcBackend.type
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  ZC {
    id: zcBackend
    equation: zcExprEdit.expression
  }

  ZcExprEdit {
    id: zcExprEdit

    anchors.left: parent.left
    anchors.right: parent.right

    optError: zcBackend.optError
  }
}
