// A line edit where the user enters an expression that evaluates to a value

import QtQuick

import ZeGrapher as ZG

Item {
  id: root

  property alias state: m_state
  property alias expression: lineEdit.text
  property alias highlighter: mhighlighter
  readonly property alias exprHeight: lineEdit.height

  implicitHeight: lineEdit.height + errorLbl.height

  function updateBorderColor() {
    if (state.status === ZG.State.NEUTRAL) {
      console.log("ZcExprEdit: border color updated to neutral")
      lineEdit.border.color = "grey";
    } else if (state.status === ZG.State.VALID) {
      console.log("ZcExprEdit: border color updated to valid")
      lineEdit.border.color = Information.appSettings.validSyntax;
    } else {
      console.log("ZcExprEdit: border color updated to invalid")
      lineEdit.border.color = Information.appSettings.invalidSyntax;
    }
  }

  ZG.State {
    id: m_state

    onUpdated: {
      console.log("state updated")
      errorLbl.setErrorMsg(m_state.errorMsg);
      root.updateBorderColor();
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
      root.updateBorderColor();
    }
  }

  Highlighter {
    id: mhighlighter
    state: m_state
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
}
