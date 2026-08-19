// A line edit for the name a math object is known by, with an optional label on top

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  // any backend with a 'name', a 'state' and setName(): NamedRef, Constant, Data
  required property QtObject backend

  property alias label: label.text

  /// @brief hint shown over the label and over the line edit
  property string hint: ""

  // where the line edit sits, for callers that align siblings with it
  readonly property alias exprY: zcExprEdit.y
  readonly property alias exprHeight: zcExprEdit.exprHeight

  spacing: 0

  // one handler over the pair, so the label and the line edit give the same
  // hint. It stays away while the field has the focus, where it would cover the
  // text
  HoverHandler { id: hoverHandler }

  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.text: root.hint
  ToolTip.visible: hoverHandler.hovered
    && !zcExprEdit.lineEditBackend.textEdit.activeFocus
    && root.hint.length !== 0

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
