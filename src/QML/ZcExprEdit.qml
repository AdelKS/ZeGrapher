// A line edit where the user enters an expression that evaluates to a value

import QtQuick

import ZeGrapher as ZG

Item {
  id: root

  property ZG.State state
  property string customErrorMsg: ''
  property bool nanValue: false

  property alias expression: lineEdit.text
  property alias highlighter: mhighlighter
  property alias lineEditBackend: lineEdit
  readonly property alias exprHeight: lineEdit.height

  signal textEdited()

  implicitHeight: lineEdit.height + errorLbl.height

  onCustomErrorMsgChanged: refresh()
  onStateChanged: refresh()
  onNanValueChanged: refresh()

  function refresh() {
    if (customErrorMsg.length !== 0)
      errorLbl.setErrorMsg(customErrorMsg);
    else {
      if (root.state.errorMsg.length === 0 && root.state.status !== ZG.State.NEUTRAL && nanValue)
        errorLbl.setErrorMsg(qsTr("NaN"));
      else errorLbl.setErrorMsg(root.state.errorMsg);
    }

    if (customErrorMsg.length !== 0 || root.state.status === ZG.State.INVALID) {
      console.debug("ZcExprEdit: border color updated to invalid")
      lineEdit.border.color = Information.appSettings.invalidSyntax;
    } else if (root.state.status === ZG.State.VALID) {
      if (nanValue) {
        console.debug("ZcExprEdit: border color updated to warning")
        lineEdit.border.color = Information.appSettings.warningSyntax;
      } else {
        console.debug("ZcExprEdit: border color updated to valid")
        lineEdit.border.color = Information.appSettings.validSyntax;
      }
    } else {
      console.debug("ZcExprEdit: border color updated to neutral")
      lineEdit.border.color = "grey";
    }
  }

  LineEdit {
    id: lineEdit
    border.color: "grey"

    anchors.left: parent.left
    anchors.right: parent.right

    onTextEdited: root.textEdited()

  }

  Connections {
    target: Information
    function onAppSettingsChanged() {
      root.refresh();
    }
  }

  Highlighter {
    id: mhighlighter
    state: root.state
    textDocument: lineEdit.textEdit.textDocument
  }

  LineEdit {
    id: errorLbl
    height: 0

    function setErrorMsg(msg: string) {
      console.debug("ZcExprEdit: new error message: ", msg);
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
}
