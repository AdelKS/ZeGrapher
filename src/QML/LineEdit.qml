import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property alias text: edit.text
  property alias textEdit: edit

  signal textEdited()

  border.width: 1.5
  radius: 4

  implicitHeight: Math.max(textMetrics.height, edit.font.pixelSize) + 2*edit.textMargin + 2*border.width

  color: palette.base

  Flickable {
    id: flick

    anchors.fill: parent
    contentWidth: Math.max(edit.contentWidth, parent.width)
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

    TextEdit {
      id: edit
      verticalAlignment: TextEdit.AlignVCenter
      anchors.fill: parent
      textMargin: 5
      color: palette.text
      focus: true
      text: ""
      onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

      onTextEdited: root.textEdited()

      Keys.onPressed: (event)=> {
        console.debug("LineEdit: key pressed, ID=0x", event.key.toString(16));
        if ([Qt.Key_Enter, Qt.Key_Return, Qt.Key_Tab].includes(event.key)) {
          // filter linebreaks events
          console.debug("LineEdit: ignoring Enter or Tab");
          event.accepted = true;
        }
      }
    }

    TextMetrics {
      id: textMetrics
      text: "=+-*/_()ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      font: edit.font
    }
  }
}
