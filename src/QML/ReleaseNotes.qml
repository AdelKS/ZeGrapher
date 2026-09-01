// The list of releases: what each one brings, and a link to its page on GitHub,
// which lists its commits. A heading of appdata/release-notes.md that covers a
// span of releases gives one entry for the whole span.
// It does not scroll. The view that holds it does.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root

  /// @brief the releases to show, newest first: {tag, span, summary, released}
  required property var entries

  /// @brief the page of a release on GitHub. The tag of the release is added
  ///        to the end of it
  readonly property string releaseUrl: "https://github.com/AdelKS/ZeGrapher/releases/tag/"

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

        // a separating line, drawn only where a release that carries notes meets
        // one that does too. Releases with no notes follow each other with no
        // line between them. The line sits at the top of the item, and the 12
        // pixels under it plus the 6 of this column make up the 18 that the
        // column above puts over it
        Item {
          visible: entry.index !== 0
                   && (entry.modelData.summary.length !== 0
                       || root.entries[entry.index - 1].summary.length !== 0)

          width: root.width
          implicitHeight: line.height + 12

          Separator {
            id: line

            width: parent.width
          }
        }

        Label {
          text: entry.modelData.tag
          font.bold: true
        }

        Label {
          // the heading of appdata/release-notes.md, which names the release the
          // notes are counted from. A heading that names one tag repeats the
          // line above, so it is hidden
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
          // a release newer than this build has no page yet. A pre-release
          // shows the span it belongs to, and that span names the release it
          // leads to
          visible: entry.modelData.released

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
