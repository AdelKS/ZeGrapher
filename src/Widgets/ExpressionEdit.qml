import QtQuick
import zegrapher.expreditbackend 1.0
import zegrapher.information 1.0

Rectangle {

  LineEdit {
    id: lineEdit
    implicitHeight: 30
    border.color: "grey"
    border.width: 1.5
    radius: 4

    anchors.left: parent.left
    anchors.right: parent.right

    Connections {
      target: Information
      function onAppSettingsChanged() {
        backend.onStateChanged(backend.state);
        backend.rehighlight();
      }
    }

    ExprEditBackend {
      id: backend
      textDocument: lineEdit.textEdit.textDocument

      onStateChanged: {
        if (state == ExprEditBackend.NEUTRAL) {
          lineEdit.border.color = "grey"
        } else if (state == ExprEditBackend.VALID) {
          lineEdit.border.color = Information.appSettings.validSyntax
        } else {
          lineEdit.border.color = Information.appSettings.invalidSyntax
        }
      }

      onErrorMsgChanged: {
        errorLbl.textEdit.text = backend.errorMsg
        if (backend.errorMsg == "") {
          console.log("Empty error message")
          errorLbl.height = 0
        } else {
          console.log("Some error message")
          errorLbl.height = errorLbl.textEdit.contentHeight + 4
        }
      }

      onValueChanged: console.log(value)
    }
  }

  LineEdit {
    id: errorLbl

    textEdit.font.pointSize: 8
    textEdit.readOnly: true
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: lineEdit.bottom

  }
}
