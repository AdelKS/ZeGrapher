// A line edit where the user enters an expression that evaluates to a value

import QtQuick

import ZeGrapher as ZG

Item {
  id: root

  required property ZcMathObject mathObj

  property alias expression: lineEdit.text
  property alias highlighter: mhighlighter
  property string customErrorMsg: ''
  readonly property alias exprHeight: lineEdit.height

  implicitHeight: lineEdit.height + errorLbl.height

  onCustomErrorMsgChanged: {
    refresh();
  }

  function refresh() {
    if (customErrorMsg.length !== 0)
      errorLbl.setErrorMsg(customErrorMsg);
    else errorLbl.setErrorMsg(mathObj.state.errorMsg);

    if (customErrorMsg.length !== 0 || mathObj.state.status === ZG.State.INVALID) {
      console.log("ZcExprEdit: border color updated to invalid")
      lineEdit.border.color = Information.appSettings.invalidSyntax;
    } else if (mathObj.state.status === ZG.State.VALID) {
      console.log("ZcExprEdit: border color updated to valid")
      lineEdit.border.color = Information.appSettings.validSyntax;
    } else {
      console.log("ZcExprEdit: border color updated to neutral")
      lineEdit.border.color = "grey";
    }
  }

  Connections {
    target: mathObj
    function onStateChanged() {
      root.refresh();
    }
  }

  LineEdit {
    id: lineEdit
    border.color: "grey"

    anchors.left: parent.left
    anchors.right: parent.right
  }

  Connections {
    target: Information
    function onAppSettingsChanged() {
      root.refresh();
    }
  }

  Highlighter {
    id: mhighlighter
    textDocument: lineEdit.textEdit.textDocument
  }

  LineEdit {
    id: errorLbl
    height: 0

    function setErrorMsg(msg: string) {
      console.log("ZcExprEdit: new error message: ", msg);
      errorLbl.textEdit.text = msg;
      height = msg ? textEdit.contentHeight + 4 : 0;
    }

    Behavior on height { SmoothedAnimation { duration: 200 } }

    radius: 4
    border.width: 0
    textEdit.textFormat: TextEdit.RichText
    textEdit.wrapMode: TextEdit.WordWrap
    textEdit.font.pointSize: 8
    textEdit.readOnly: true
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: lineEdit.bottom
  }

  Component.onCompleted: {
    mhighlighter.mathObj = mathObj;
    mathObj.highlighter = mhighlighter;
  }
}
