import QtQuick
import zegrapher.expreditbackend 1.0

Rectangle {
  color: 'white'
  implicitHeight: 29
  border.color: 'green'

  LineEdit {
    id: lineEdit
    ExprEditBackend {
      id: backend
      textDocument: lineEdit.textEdit.textDocument
    }
  }
}
