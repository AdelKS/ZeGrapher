// A line edit for the name a math object is known by, with an optional label on top

import QtQuick
import QtQuick.Layouts

ColumnLayout {
  id: root

  // any backend with a 'name', a 'state' and setName(): NamedRef, Constant, Data
  required property QtObject backend

  property alias label: label.text

  // where the line edit sits, for callers that align siblings with it
  readonly property alias exprY: zcExprEdit.y
  readonly property alias exprHeight: zcExprEdit.exprHeight

  spacing: 0

  ZeLabel {
    id: label

    visible: text.length !== 0
    Layout.alignment: Qt.AlignHCenter
  }

  ZcExprEdit {
    id: zcExprEdit

    state: root.backend.state

    expression: root.backend.name
    onTextEdited: (expression) => root.backend.setName(expression)

    Layout.fillWidth: true
    Layout.alignment: Qt.AlignTop
  }
}
