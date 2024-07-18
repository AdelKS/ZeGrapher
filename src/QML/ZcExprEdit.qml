// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import zegrapher.highlighter 1.0
import zegrapher.information 1.0
import zegrapher.opterror 1.0

Item {
  id: root

  property OptError optError
  property alias expression: lineEdit.text
  property alias highlighter: mhighlighter
  readonly property alias exprHeight: lineEdit.height

  implicitHeight: lineEdit.height + errorLbl.height

  function setCustomErrorMsg(msg: string) {
    errorLbl.setErrorMsg(msg);
    lineEdit.border.color = Information.appSettings.invalidSyntax;
  }

  onOptErrorChanged: {
    console.log("ZcExprEdit: OptError update")
    if (optError.type === OptError.NEUTRAL)
    {
      errorLbl.setErrorMsg("");
      lineEdit.border.color = "grey";
    }
    else if (optError.type === OptError.VALID)
    {
      errorLbl.setErrorMsg("");
      lineEdit.border.color = Information.appSettings.validSyntax;
    }
    else
    {
      errorLbl.setErrorMsg(optError.getMessage());
      lineEdit.border.color = Information.appSettings.invalidSyntax;
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
      highlighter.rehighlight();
    }
  }

  Highlighter {
    id: mhighlighter
    optError: root.optError
    textDocument: lineEdit.textEdit.textDocument
  }

  LineEdit {
    id: errorLbl
    height: 0

    function setErrorMsg(msg: string) {
      console.log("ZcExprEdit: new error message: ", msg);
      textEdit.text = msg;
      height = msg ?  textEdit.contentHeight + 4 : 0;
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
