// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Controls.FluentWinUI3

import ZeGrapher as ZG

Item {
  id: root

  property zgState state
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

  onCustomErrorMsgChanged: refresh()
  onStateChanged: refresh()
  onNanValueChanged: refresh()

  function refresh() {
    if (customErrorMsg.length !== 0)
      errorLbl.setErrorMsg(customErrorMsg);
    else {
      if (root.state.errorMsg.length === 0 && root.state.status !== ZG.State.NEUTRAL && nanValue)
        errorLbl.setErrorMsg("NaN");
      else errorLbl.setErrorMsg(root.state.errorMsg);
    }
  }

  LineEdit {
    id: lineEdit
    border.color: {
      if (root.customErrorMsg.length !== 0 || root.state.status === ZG.State.INVALID)
        return ZeStyle.themed(Information.appSettings.invalidSyntax);
      if (root.state.status === ZG.State.VALID)
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
    state: root.state
    textDocument: lineEdit.textEdit.textDocument
  }

  LineEdit {
    id: errorLbl
    height: textEdit.length > 0 ? implicitHeight + 4 : 0

    function setErrorMsg(msg: string) {
      errorLbl.textEdit.text = msg;
    }

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
