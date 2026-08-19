// The manual that ':/website/<lang>/' holds, folded over the graph

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Effects

Rectangle {
  id: root

  color: myPalette.window

  /// @brief whether the pane is unfolded, which its own button toggles
  property alias open: docs_button.checked

  /// @brief where the button stands, under the button of the panel
  property real buttonTopMargin

  /// @brief how far a heading stands from the edge of the pane
  readonly property int headingMargin: 14

  /// @brief how far the text and the pictures of a section stand in from a heading
  readonly property int contentMargin: 26

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  RectangularShadow {
    z: -2
    anchors.fill: docs_button
    radius: docs_button.radius
    blur: 10
    spread: 0
    color: myPalette.shadow
  }

  // the button of the panel shows an arrow, for the direction it folds in. This
  // one shows a book, for what it unfolds
  Rectangle {
    id: docs_button

    width: 25
    height: width
    radius: 8

    // over the background of the pane, under its page: the text and the
    // scrollbar cover the half of the button that lies inside the pane, and
    // only the half outside it stays visible
    z: 0

    // the color of the page it opens, so the button and the pane match
    color: root.color

    property int apparentWidth: 2. * width / 3.
    property bool checked: false

    anchors.top: parent.top
    anchors.topMargin: root.buttonTopMargin
    anchors.right: parent.right
    anchors.rightMargin: -apparentWidth

    Image {
      anchors.centerIn: parent
      // centered on the visible half of the button, not on the button
      anchors.horizontalCenterOffset: docs_button.apparentWidth / 4 - 1
      width: 14
      height: 14

      source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/bookmark-dark.svg"
                                                              : "qrc:/icons/bookmark-light.svg"
      fillMode: Image.PreserveAspectFit
      mipmap: true
    }

    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      cursorShape: Qt.PointingHandCursor
      onClicked: docs_button.checked = !docs_button.checked

      ToolTip.delay: ZeStyle.tooltipDelay
      ToolTip.text: docs_button.checked ? qsTr("Hide the documentation")
                                        : qsTr("Show the documentation")
      ToolTip.visible: containsMouse
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

  // the manual is one page, so a link inside it points at a heading of that
  // page. Anything else leaves the app
  function follow(link) {
    if (link.startsWith('#'))
      root.jumpTo(link);
    else
      Qt.openUrlExternally(link);
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

      TextArea {
        property var modelData

        // a heading stands alone in its block, and keeps space around itself
        readonly property bool heading: modelData ? modelData.anchor.length !== 0 : false

        readOnly: true
        selectByMouse: true
        background: null
        font: Information.appSettings.font
        textFormat: TextEdit.MarkdownText
        wrapMode: Text.WordWrap
        color: myPalette.text
        topPadding: heading ? 24 : 0
        bottomPadding: heading ? 8 : 0
        leftPadding: heading ? root.headingMargin : root.contentMargin
        rightPadding: heading ? root.headingMargin : root.contentMargin

        text: modelData ? modelData.text : ""

        onLinkActivated: (link) => root.follow(link)

        MouseArea {
          anchors.fill: parent
          acceptedButtons: Qt.NoButton
          cursorShape: parent.hoveredLink.length !== 0 ? Qt.PointingHandCursor : Qt.IBeamCursor
        }
      }
    }

    Component {
      id: asPicture

      // the pictures come from captures of a screen that draws two pixels per
      // logical one, so each one shows at half its size, and no wider than the page
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

          width: modelData ? Math.min(modelData.width / 2, column.width - 2 * root.contentMargin) : 0
          height: modelData && modelData.width > 0 ? modelData.height * (width / modelData.width) : 0
        }
      }
    }

    // a wheel notch moves three lines of text. The flickable alone moves a
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
