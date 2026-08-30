// A block of markdown. Its links have the color of a valid expression, and
// change color under the pointer.
// It is drawn in a read-only TextArea and not in a Label, because a Label draws
// with Text, which has no QTextDocument. The indent width of a markdown list is
// a setting of that document.

import QtQuick
import QtQuick.Controls.FluentWinUI3

TextArea {
  id: root

  /// @brief the markdown to draw, with its links written as [label](url)
  required property string markdown

  /// @brief the link under the pointer, empty when the pointer is over none.
  ///        TextEdit.hoveredLink stays empty here, so linkAt() reads the link
  ///        at the position the hover handler reports
  readonly property string pointedLink: pointer.hovered
                                        ? root.linkAt(pointer.point.position.x,
                                                      pointer.point.position.y)
                                        : ""

  readOnly: true
  selectByMouse: false
  activeFocusOnPress: false
  background: null

  // the style of a TextArea sets each padding and each inset on its own, so a
  // single `padding: 0` leaves them in place. QQuickTextEdit::linkAt() also adds
  // the padding to the position it reads, and swaps its two sides. Each padding
  // and each inset is therefore set to 0 here
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

  // Text.linkColor never reaches a link of markdown: QTextMarkdownImporter gives
  // every link the Link color of the palette, and QQuickTextNodeEngine keeps the
  // color of the character over the color of the item. An <a> tag of HTML carries
  // its own color, and markdown accepts HTML inside it, so each link is rewritten
  // as an <a> tag here
  text: root.markdown.replace(/\[([^\]]*)\]\(([^)]*)\)/g, (match, label, url) =>
          '<a href="' + url + '" style="color:'
          + (url === root.pointedLink ? ZeStyle.hoveredLinkColor : ZeStyle.linkColor)
          + '">' + label + '</a>')

  Component.onCompleted: TextDocumentTools.setIndentWidth(root.textDocument,
                                                          ZeStyle.listIndent)

  HoverHandler {
    id: pointer

    cursorShape: root.pointedLink.length !== 0 ? Qt.PointingHandCursor
               : root.selectByMouse ? Qt.IBeamCursor : Qt.ArrowCursor
  }
}
