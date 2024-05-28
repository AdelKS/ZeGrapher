import QtQuick

// A line edit without borders, that follows its content

Rectangle {
  id: rec
  property TextEdit textEdit: edit

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  border.width: 1.5
  radius: 4

  height: Math.max(textMetrics.height, edit.font.pixelSize) + 2*edit.textMargin + border.width

  color: myPalette.base

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
      color: myPalette.text
      focus: true
      text: ""
      onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
    }

    TextMetrics {
      id: textMetrics
      text: edit.text
    }
  }
}
