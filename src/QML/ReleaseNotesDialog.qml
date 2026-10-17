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

// The dialog shown at the first start after an update: every release since the
// last version that ran, with the site's donation appeal below.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Dialog {
  id: root

  /// @brief the releases to show. MainWindow opens the dialog when this list is
  ///        not empty
  readonly property var entries: BuildInfo.releaseNotesSince(Information.lastRunVersion)

  title: qsTr("What's new in ZeGrapher %1").arg(Application.version)

  modal: true
  closePolicy: Popup.CloseOnEscape
  anchors.centerIn: parent

  width: Math.min(560, parent.width - 60)
  height: Math.min(implicitHeight, parent.height - 60)

  // the title of a Dialog is a line of text, so the logo is drawn beside it here
  header: RowLayout {
    spacing: 12

    Image {
      Layout.leftMargin: 20
      Layout.topMargin: 20

      source: "qrc:/icons/ZeGrapher.svg"
      sourceSize: Qt.size(40, 40)
      fillMode: Image.PreserveAspectFit
      mipmap: true
    }

    Label {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

      text: root.title
      font.bold: true
      color: ZeStyle.palette.windowText
      wrapMode: Text.WordWrap
    }
  }

  contentItem: ColumnLayout {
    spacing: 14

    ScrollView {
      id: notesView

      Layout.fillWidth: true
      Layout.fillHeight: true

      // the notes of one release fit without a scrollbar, and a long list of
      // releases scrolls instead of making the dialog taller
      Layout.preferredHeight: notes.implicitHeight
      Layout.maximumHeight: 320
      // filling the leftover height leaves nothing in a short dialog, but the
      // releases always need a few lines. This floor never exceeds their
      // implicit height, so a single short release stays short
      Layout.minimumHeight: Math.min(120, notes.implicitHeight)

      clip: true
      // the scrollbar sits at the right edge of the view, over the padding
      rightPadding: notesView.ScrollBar.vertical.width + ZeStyle.scrollBarGap

      ScrollBar.vertical.policy: ScrollBar.AsNeeded

      ReleaseNotes {
        id: notes

        width: notesView.availableWidth
        entries: root.entries
      }
    }

    Separator {
      Layout.fillWidth: true
    }

    DonationBox {
      Layout.fillWidth: true
    }

    Button {
      Layout.alignment: Qt.AlignRight
      text: qsTr("Close")
      onClicked: root.close()
    }
  }
}
