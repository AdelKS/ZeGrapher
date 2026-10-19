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

// A line edit for an expression: its border color shows the expression's
// state, and its error message shows below

import QtQuick
import QtQuick.Controls.FluentWinUI3

import ZeGrapher as ZG

Item {
  id: root

  property zgState exprState
  property string customErrorMsg: ''
  property bool nanValue: false

  /// @brief the line edit's hint
  property string hint

  property alias highlighter: mhighlighter
  property alias lineEditBackend: lineEdit
  readonly property alias exprHeight: lineEdit.height

  property string expression
  signal textEdited(string expression)

  onExpressionChanged: if (lineEdit.text !== expression) lineEdit.text = expression;
  Component.onCompleted: lineEdit.text = expression;

  implicitHeight: lineEdit.height + errorLbl.height

  readonly property string errorMsg: {
    if (root.customErrorMsg.length !== 0)
      return root.customErrorMsg;
    if (root.exprState.errorMsg.length === 0 && root.exprState.status !== ZG.State.NEUTRAL && root.nanValue)
      return "NaN";
    return root.exprState.errorMsg;
  }

  LineEdit {
    id: lineEdit
    border.color: {
      if (root.customErrorMsg.length !== 0 || root.exprState.status === ZG.State.INVALID)
        return ZeStyle.themed(Information.appSettings.invalidSyntax);
      if (root.exprState.status === ZG.State.VALID)
        return ZeStyle.themed(root.nanValue ? Information.appSettings.warningSyntax
                                            : Information.appSettings.validSyntax);
      return "grey";
    }

    ToolTip.text: root.hint

    anchors.left: parent.left
    anchors.right: parent.right

    onTextEdited: root.textEdited(lineEdit.text)

  }

  Highlighter {
    id: mhighlighter
    state: root.exprState
    textDocument: lineEdit.textEdit.textDocument
  }

  LineEdit {
    id: errorLbl
    height: textEdit.length > 0 ? implicitHeight + 4 : 0

    text: root.errorMsg

    Behavior on height { SmoothedAnimation { duration: 200 } }

    radius: 4
    border.width: 0
    textEdit.textFormat: TextEdit.RichText
    textEdit.wrapMode: TextEdit.WordWrap
    textEdit.readOnly: true
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: lineEdit.bottom
  }
}
