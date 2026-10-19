/****************************************************************************
**  Copyright (c) 2023, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
      activeFocusOnTab: !readOnly
      text: ""
      onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)

      onTextEdited: {
        // Keys.onPressed below filters Enter, but not a pasted line break
        if (/[\r\n\u2029]/.test(text)) {
          const position = cursorPosition;
          text = text.replace(/\r\n|[\r\n\u2029]/g, " ");
          cursorPosition = Math.min(position, length);
        }
        rec.textEdited();
      }

      Keys.onPressed: (event)=> {
        if ([Qt.Key_Enter, Qt.Key_Return].includes(event.key)) {
          // filter linebreaks events
          event.accepted = true;
        } else if ([Qt.Key_Tab, Qt.Key_Backtab].includes(event.key)) {
          // TextEdit writes a tab character instead of moving the focus
          nextItemInFocusChain(event.key === Qt.Key_Tab).forceActiveFocus(Qt.TabFocusReason);
          event.accepted = true;
        }
      }
    }
  }
}
