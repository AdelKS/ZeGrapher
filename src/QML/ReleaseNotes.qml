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
  readonly property string releaseUrl: BuildInfo.repository + "/releases/tag/"

  implicitWidth: column.implicitWidth
  implicitHeight: column.implicitHeight

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

        // a separating line between two releases. The line sits at the top of
        // the item, and the 12 pixels under it plus the 6 of this column make up
        // the 18 that the column above puts over it
        Item {
          visible: entry.index !== 0

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
          // notes are counted from. It is empty for a heading of one release
          visible: entry.modelData.span.length !== 0

          text: entry.modelData.span
          color: ZeStyle.palette.text
          opacity: 0.7
        }

        LinkLabel {
          width: root.width
          visible: markdown.length !== 0

          markdown: entry.modelData.summary
        }

        LinkLabel {
          // a release newer than this build has no page yet. A pre-release
          // shows the span it belongs to, and that span names the release it
          // leads to
          visible: entry.modelData.released

          //: opens the page of a release on GitHub, which lists its commits
          markdown: "[" + qsTr("See the changes on GitHub") + "](" + root.releaseUrl + entry.modelData.tag + ")"
        }
      }
    }
  }
}
