// A block of markdown whose links take the color of a valid expression, and
// change color under the pointer.
// It draws in a read-only TextArea and not in a Label: the step that a markdown
// list indents by lives in the QTextDocument, which Text carries none of.

import QtQuick
import QtQuick.Controls.FluentWinUI3

TextArea {
  id: root

  /// @brief the markdown to draw, with its links written as [label](url)
  required property string markdown

  /// @brief the link under the pointer, empty when the pointer is over none.
  ///        hoveredLink of TextEdit stays empty here, and linkAt() reads the
  ///        link from the position that the handler gives
  readonly property string pointedLink: pointer.hovered
                                        ? root.linkAt(pointer.point.position.x,
                                                      pointer.point.position.y)
                                        : ""

  readOnly: true
  selectByMouse: false
  activeFocusOnPress: false
  background: null

  // the style of a TextArea writes each padding and each inset of its own, and
  // a `padding: 0` alone leaves them in place. linkAt() of QQuickTextEdit also
  // adds the padding to the position it reads, and swaps the two sides of it
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

  // Text.linkColor never reaches a link of markdown: QTextMarkdownImporter
  // gives every link the Link color of the palette, and QQuickTextNodeEngine
  // keeps the color of the character over the color of the item. An anchor of
  // html carries its own color, and markdown draws one among its own marks
  text: root.markdown.replace(/\[([^\]]*)\]\(([^)]*)\)/g, (match, label, url) =>
          '<a href="' + url + '" style="color:'
          + (url === root.pointedLink ? ZeStyle.hoveredLinkColor : ZeStyle.linkColor)
          + '">' + label + '</a>')

  Component.onCompleted: TextDocumentTools.setIndentWidth(root.textDocument,
                                                          ZeStyle.listIndent)

  HoverHandler {
    id: pointer

    cursorShape: root.pointedLink.length !== 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
  }
}
