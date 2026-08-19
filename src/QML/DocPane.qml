// The manual in the interface language, in a pane that unfolds over the graph.
// Documentation (C++) reads it from ':/website/<lang>/'.

import QtQml.Models
import QtQuick
import QtQuick.Controls.FluentWinUI3

Rectangle {
  id: root

  color: ZeStyle.palette.window

  /// @brief true while the pane is unfolded. Its own button toggles it
  property alias open: docs_button.checked

  /// @brief the button's y position, under the panel's fold button
  property real buttonTopMargin

  /// @brief the device pixels per logical pixel of the manual's pictures: the
  ///        SCALE of the screen website/scripts/make-images.py captures them on
  readonly property real pictureScale: 1.6

  /// @brief the margin between a heading and the edge of the pane
  readonly property int headingMargin: 14

  /// @brief how far a section's text and pictures are indented from its
  ///        heading
  readonly property int contentMargin: 26

  /// @brief the manual in the interface language, one map per block
  readonly property var manual: Documentation.blocks(Information.appSettings.language)

  onManualChanged: scroll.contentItem.contentY = 0

  // the panel's button shows an arrow pointing where it folds. This one shows a
  // book, because it opens the manual
  EdgeButton {
    id: docs_button

    // drawn over the pane's background and under its page, so the text and
    // scrollbar cover the part of the button inside the pane
    z: 0

    anchors.top: parent.top
    anchors.topMargin: root.buttonTopMargin

    ToolTip.text: docs_button.checked ? qsTr("Hide the documentation")
                                     : qsTr("Show the documentation")

    Image {
      anchors.centerIn: parent
      // centered on the visible half of the button, not on the button
      anchors.horizontalCenterOffset: docs_button.apparentWidth / 4 - 1
      width: 14
      height: 14

      source: ZeStyle.icon("bookmark")
      fillMode: Image.PreserveAspectFit
      mipmap: true
    }
  }

  function jumpTo(anchor) {
    const index = root.manual.findIndex((block) => "#" + block.anchor === anchor);
    const block = index < 0 ? null : blocks.itemAt(index);

    if (block === null) {
      console.warn("DocPane: no heading has the anchor", anchor);
      return;
    }

    const flickable = scroll.contentItem;
    flickable.contentY = Math.min(block.y, Math.max(flickable.contentHeight - flickable.height, 0));
  }

  ScrollView {
    id: scroll

    z: 1

    anchors.fill: parent

    clip: true
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    Column {
      id: column

      width: scroll.availableWidth
      topPadding: 40
      bottomPadding: 40

      Repeater {
        id: blocks
        model: root.manual

        DelegateChooser {
          role: "kind"

          // LinkLabel has no padding, so the label's position sets the space
          // around the text. A heading is alone in its block, so this also sets
          // the space around headings
          DelegateChoice {
            roleValue: "text"

            Item {
              id: textBlock

              required property var modelData

              readonly property bool heading: modelData.anchor.length !== 0
              readonly property real sideMargin: heading ? root.headingMargin : root.contentMargin

              width: column.width
              implicitHeight: label.y + label.implicitHeight + (heading ? 8 : 0)

              LinkLabel {
                id: label

                x: textBlock.sideMargin
                y: textBlock.heading ? 24 : 0
                width: textBlock.width - 2 * textBlock.sideMargin

                markdown: textBlock.modelData.text
                selectByMouse: true
                activeFocusOnPress: true

                onAnchorActivated: (anchor) => root.jumpTo(anchor)
              }
            }
          }

          // each picture is drawn at its on-screen window size, and never wider
          // than the page
          DelegateChoice {
            roleValue: "image"

            Item {
              id: pictureBlock

              required property var modelData

              width: column.width
              implicitHeight: shown.height + 16

              Image {
                id: shown

                x: (parent.width - width) / 2
                y: 8
                source: pictureBlock.modelData.source
                fillMode: Image.PreserveAspectFit
                mipmap: true

                width: Math.min(pictureBlock.modelData.width / root.pictureScale,
                                column.width - 2 * root.contentMargin)
                height: pictureBlock.modelData.width > 0
                        ? pictureBlock.modelData.height * (width / pictureBlock.modelData.width) : 0
              }
            }
          }
        }
      }
    }

    // one wheel notch scrolls three lines of text. The Flickable alone scrolls a
    // fraction of that
    WheelHandler {
      acceptedDevices: PointerDevice.Mouse
      onWheel: (event) => {
        const flickable = scroll.contentItem;
        const step = 3 * Information.appSettings.font.pixelSize * 1.6;

        flickable.contentY = Math.max(0,
          Math.min(flickable.contentHeight - flickable.height,
                   flickable.contentY - event.angleDelta.y / 120 * step));
      }
    }
  }

}
