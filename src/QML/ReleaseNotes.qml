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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// The list of releases: what each one brings, with a link to its GitHub page,
// which lists its commits. Each announcement and changes entry of
// appdata/releases.yaml gives one item, for its whole span.
// It doesn't scroll: the view that holds it does.

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.FluentWinUI3

Column {
  id: root

  /// @brief the releases to show, newest first: {tag, span, summary, released}
  required property var entries

  /// @brief the base URL of GitHub release pages, which each release appends
  ///        its tag to
  readonly property string releaseUrl: BuildInfo.repository + "/releases/tag/"

  spacing: 18

  Repeater {
    model: root.entries

    Column {
      id: entry

      required property var modelData
      required property int index

      spacing: 6

      // a line between two releases, at the top of the item. The 12 pixels below
      // it plus this column's 6 match the 18 that the column above puts over it
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
        // the span of the entry, which names the release the notes count
        // from. It is empty for an entry about one release
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
        // shows the announcement of the release it leads to
        visible: entry.modelData.released

        //: opens the page of a release on GitHub, which lists its commits
        markdown: "[" + qsTr("See the changes on GitHub") + "](" + root.releaseUrl + entry.modelData.tag + ")"
      }
    }
  }
}
