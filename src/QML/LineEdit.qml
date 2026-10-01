import QtQuick
import QtQuick.Controls.FluentWinUI3

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias text: edit.text
  property alias textEdit: edit

  signal textEdited()

  // call sites set the hint with ToolTip.text. It hides while the field has
  // focus, where it would cover the text
  HoverHandler { id: hoverHandler }

  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hoverHandler.hovered && !edit.activeFocus && ToolTip.text.length !== 0

  border.width: 1.5
  radius: 4

  border.color: "grey"

  implicitHeight: Math.max(edit.contentHeight, edit.font.pixelSize) + 10 + 2*border.width

  color: palette.base

  Flickable {
    id: flick

    anchors.fill: parent
    anchors.leftMargin: 6
    anchors.rightMargin: 6
    // a wrapped text takes the width it is given, and its contentWidth follows
    // that width
    contentWidth: edit.wrapMode === TextEdit.NoWrap ? Math.max(edit.contentWidth, width) : width
    contentHeight: Math.max(edit.contentHeight, parent.height)
    clip: true
    interactive: false

    function ensureVisible(r)
    {
      if (contentX >= r.x)
        contentX = r.x;
      else if (contentX+width <= r.x+r.width)
        contentX = r.x+r.width-width;
      if (contentY >= r.y)
        contentY = r.y;
      else if (contentY+height <= r.y+r.height)
        contentY = r.y+r.height-height;
    }

    ZeTextEdit {
      id: edit
      verticalAlignment: TextEdit.AlignVCenter
      anchors.fill: parent
      color: palette.text
      focus: true
      text: ""
      onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

      onTextEdited: rec.textEdited()

      Keys.onPressed: (event)=> {
        if ([Qt.Key_Enter, Qt.Key_Return, Qt.Key_Tab].includes(event.key)) {
          // filter linebreaks events
          event.accepted = true;
        }
      }
    }
  }
}
