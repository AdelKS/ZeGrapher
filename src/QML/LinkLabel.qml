/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// A markdown block, with links in the valid-expression color that change color
// on hover.
// It draws in a read-only TextArea instead of a Label, because a Label draws
// with Text, which has no QTextDocument for TextDocumentTools to edit.

import QtQuick
import QtQuick.Controls.FluentWinUI3

TextArea {
  id: root

  /// @brief the markdown to draw, with its links written as [label](url)
  required property string markdown

  /// @brief emitted for a link to '#anchor'. Every other link opens with
  ///        Qt.openUrlExternally()
  signal anchorActivated(string anchor)

  /// @brief the link under the pointer, empty if there is none.
  ///        TextEdit.hoveredLink stays empty here, so linkAt() reads the link
  ///        at the hover handler's position
  readonly property string pointedLink: pointer.hovered
                                        ? root.linkAt(pointer.point.position.x,
                                                      pointer.point.position.y)
                                        : ""

  readOnly: true
  selectByMouse: false
  activeFocusOnPress: false
  background: null
  color: ZeStyle.palette.text

  // the TextArea style sets each padding and inset separately, so a single
  // `padding: 0` leaves them in place. QQuickTextEdit::linkAt() also adds the
  // padding to the position it reads, with its two sides swapped. So set every
  // padding and inset to 0 here
  topPadding: 0
  bottomPadding: 0
  leftPadding: 0
  rightPadding: 0
  topInset: 0
  bottomInset: 0
  leftInset: 0
  rightInset: 0

  textFormat: TextEdit.MarkdownText
  wrapMode: Text.WordWrap
  verticalAlignment: Text.AlignTop

  text: root.markdown

  function colorLinks() {
    TextDocumentTools.colorLinks(root.textDocument, ZeStyle.linkColor,
                                 ZeStyle.hoveredLinkColor, root.pointedLink);
  }

  onTextChanged: root.colorLinks()
  onPointedLinkChanged: root.colorLinks()

  Connections {
    target: ZeStyle

    function onLinkColorChanged() {
      root.colorLinks();
    }

    function onHoveredLinkColorChanged() {
      root.colorLinks();
    }
  }

  onLinkActivated: (link) => {
    if (link.startsWith('#'))
      root.anchorActivated(link);
    else
      Qt.openUrlExternally(link);
  }

  Component.onCompleted: TextDocumentTools.setIndentWidth(root.textDocument,
                                                          ZeStyle.listIndent)

  HoverHandler {
    id: pointer

    cursorShape: root.pointedLink.length !== 0 ? Qt.PointingHandCursor
               : root.selectByMouse ? Qt.IBeamCursor : Qt.ArrowCursor
  }
}
