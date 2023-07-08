import QtQuick

// A line edit without borders, that follows its content

Flickable {
  id: flick

  property TextEdit textEdit: edit

  anchors.fill: parent
  anchors.margins: 2
  contentWidth: edit.contentWidth
  contentHeight: edit.contentHeight
  clip: true

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
    anchors.margins: 2
    focus: true
    // wrapMode: TextEdit.Wrap
    text: ""
    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

  }
}
