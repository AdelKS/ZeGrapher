import QtQuick

// A line edit without borders, that follows its content

Rectangle {

  property TextEdit textEdit: edit

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.base

  Flickable {
    id: flick

    anchors.fill: parent
    anchors.margins: 2
    contentWidth: Math.max(edit.contentWidth, parent.width)
    contentHeight: Math.max(edit.contentHeight, parent.width)
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
      color: myPalette.text
      focus: true
      // wrapMode: TextEdit.Wrap
      text: ""
      onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

    }
  }
}
