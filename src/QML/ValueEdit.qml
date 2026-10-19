/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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

    exprState: root.backend.state
  }
}
