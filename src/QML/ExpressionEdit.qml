import QtQuick
import zegrapher.expreditbackend 1.0
import zegrapher.information 1.0

Item {
  id: root

  property string expression
  property string errorMsg
  property int state
  property double value: 0
  property int exprHeight: lineEdit.height
  property alias type: backend.type

  implicitHeight: lineEdit.height + errorLbl.height

  function setCustomErrorMsg(customErrMsg) {
    errorMsg = customErrMsg
    state = ExprEditBackend.INVALID
  }

  onExpressionChanged: lineEdit.textEdit.text = expression
  onErrorMsgChanged: {
    errorLbl.textEdit.text = errorMsg

    if (errorMsg === "") {
      errorLbl.height = 0
    } else {
      errorLbl.height = errorLbl.textEdit.contentHeight + 4
    }
  }

  onStateChanged: {
    if (state === ExprEditBackend.NEUTRAL) {
      lineEdit.border.color = "grey"
    } else if (state === ExprEditBackend.VALID) {
      lineEdit.border.color = Information.appSettings.validSyntax
    } else {
      lineEdit.border.color = Information.appSettings.invalidSyntax
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
      backend.rehighlight();
    }
  }

  ExprEditBackend {
    id: backend
    textDocument: lineEdit.textEdit.textDocument
    onErrorMsgChanged: root.errorMsg = backend.errorMsg
    onValueChanged: {
      root.errorMsg = ""
      root.state = ExprEditBackend.VALID
      root.value = value
    }
    onStateChanged: root.state = state
  }

  LineEdit {
    id: errorLbl
    height: 0

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
