// What each release brings, and a link to the page of that release on GitHub,
// which lists its commits. A word of appdata/release-notes.md that covers a
// span of releases draws them as one entry. WhatsNewDialog and About both draw
// this.
// It scrolls nothing: the view that holds it does that.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root

  /// @brief the entries to draw, newest first: {tag, span, summary}
  required property var entries

  /// @brief the page of one release, its tag written under it
  readonly property string releaseUrl: "https://github.com/AdelKS/ZeGrapher/releases/tag/"

  /// @brief whether this build runs a version that no release carries. GitHub
  ///        then holds no page to open, and the link of every entry drops out
  readonly property bool pending: Application.version.endsWith("-dev")

  implicitWidth: column.implicitWidth
  implicitHeight: column.implicitHeight

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  Column {
    id: column

    width: root.width
    spacing: 18

    Repeater {
      model: root.entries

      Column {
        id: entry

        required property var modelData
        required property int index

        spacing: 6

        // a line where a word of the author starts or ends. The releases that
        // carry none run on with no line between them. The line sits at the top
        // of the item: the 12 pixels under it plus the 6 of this column answer
        // the 18 that the column above puts over it
        Item {
          visible: entry.index !== 0
                   && (entry.modelData.summary.length !== 0
                       || root.entries[entry.index - 1].summary.length !== 0)

          width: root.width
          implicitHeight: line.height + 12

          Rectangle {
            id: line

            width: parent.width
            height: ZeStyle.separatorThickness
            color: myPalette.mid
          }
        }

        Label {
          text: entry.modelData.tag
          font.bold: true
        }

        Label {
          // the heading of appdata/release-notes.md, which names the release
          // the word counts from. A heading of one tag repeats the line above
          visible: entry.modelData.span.length !== 0
                   && entry.modelData.span !== entry.modelData.tag

          text: entry.modelData.span
          color: myPalette.text
          opacity: 0.7
        }

        LinkLabel {
          width: root.width
          visible: markdown.length !== 0

          markdown: entry.modelData.summary
          color: myPalette.text

          onLinkActivated: (link) => Qt.openUrlExternally(link)
        }

        Label {
          visible: !root.pending

          //: opens the page of a release on GitHub, which lists its commits
          text: qsTr("See the changes on GitHub")
          // the color of the links beside it, and of the pointer over them
          color: linkArea.containsMouse ? ZeStyle.hoveredLinkColor : ZeStyle.linkColor

          MouseArea {
            id: linkArea

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: Qt.openUrlExternally(root.releaseUrl + entry.modelData.tag)
          }
        }
      }
    }
  }
}
