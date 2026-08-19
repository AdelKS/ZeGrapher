// The manual in the language of the interface. It is drawn in a pane that
// unfolds over the graph. Documentation (C++) reads it from ':/website/<lang>/'.

import QtQuick
import QtQuick.Controls.FluentWinUI3

Rectangle {
  id: root

  color: ZeStyle.palette.window

  /// @brief true while the pane is unfolded. Its own button toggles it
  property alias open: docs_button.checked

  /// @brief the y position of the button, under the button that folds the panel
  property real buttonTopMargin

  /// @brief the device pixels per logical pixel of the pictures of the manual.
  ///        It is the SCALE of the screen that website/scripts/make-images.py
  ///        takes them on
  readonly property real pictureScale: 1.6

  /// @brief the margin between a heading and the edge of the pane
  readonly property int headingMargin: 14

  /// @brief how far the text and the pictures of a section are indented from
  ///        their heading
  readonly property int contentMargin: 26

  // the button of the panel shows an arrow that gives the direction it folds in.
  // This one shows a book, because it opens the manual
  EdgeButton {
    id: docs_button

    // drawn over the background of the pane and under its page, so the text and
    // the scrollbar cover the part of the button that lies inside the pane
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

      source: ZeStyle.dark ? "qrc:/icons/bookmark-light.svg" : "qrc:/icons/bookmark-dark.svg"
      fillMode: Image.PreserveAspectFit
      mipmap: true
    }
  }

  function jumpTo(anchor) {
    const index = docs.blockOfAnchor(anchor);
    const block = index < 0 ? null : blocks.itemAt(index);

    if (block === null) {
      console.warn("DocPane: no heading has the anchor", anchor);
      return;
    }

    const flickable = scroll.contentItem;
    flickable.contentY = Math.min(block.y, Math.max(flickable.contentHeight - flickable.height, 0));
  }

  Documentation {
    id: docs

    language: Information.appSettings.language

    onBlocksChanged: scroll.contentItem.contentY = 0
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
        model: docs.blocks

        Loader {
          required property var modelData

          width: column.width
          sourceComponent: modelData.kind === "image" ? asPicture : asText

          onLoaded: item.modelData = modelData
        }
      }
    }

    Component {
      id: asText

      // LinkLabel takes no padding, so the space around the text is set by the
      // position of the label. A heading is alone in its block, so the space
      // around a heading is set here too
      Item {
        id: textBlock

        property var modelData

        readonly property bool heading: modelData ? modelData.anchor.length !== 0 : false
        readonly property real sideMargin: heading ? root.headingMargin : root.contentMargin

        implicitHeight: label.y + label.implicitHeight + (heading ? 8 : 0)

        LinkLabel {
          id: label

          x: textBlock.sideMargin
          y: textBlock.heading ? 24 : 0
          width: textBlock.width - 2 * textBlock.sideMargin

          markdown: textBlock.modelData ? textBlock.modelData.text : ""
          selectByMouse: true
          activeFocusOnPress: true
          font: Information.appSettings.font

          onAnchorActivated: (anchor) => root.jumpTo(anchor)
        }
      }
    }

    Component {
      id: asPicture

      // each picture is drawn at the size the window had on the screen, and never
      // wider than the page
      Item {
        property var modelData

        implicitHeight: shown.height + 16

        Image {
          id: shown

          x: (parent.width - width) / 2
          y: 8
          source: modelData ? modelData.source : ""
          fillMode: Image.PreserveAspectFit
          mipmap: true

          width: modelData ? Math.min(modelData.width / root.pictureScale, column.width - 2 * root.contentMargin) : 0
          height: modelData && modelData.width > 0 ? modelData.height * (width / modelData.width) : 0
        }
      }
    }

    // one notch of the wheel scrolls three lines of text. The Flickable on its
    // own scrolls a fraction of that
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
