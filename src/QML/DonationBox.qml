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

// The site's donation appeal, and the button that opens the donation page.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  spacing: 10

  LinkLabel {
    Layout.fillWidth: true

    markdown: Donation.words(Information.appSettings.language)
  }

  Button {
    Layout.alignment: Qt.AlignCenter

    highlighted: true
    text: qsTr("Donate")

    // the same icon as the site's donation panel. Its viewBox is 576x512, so a
    // box with another ratio would stretch it
    icon.source: "qrc:/icons/hand-holding-heart.svg"
    icon.width: 18
    icon.height: 16

    onClicked: Qt.openUrlExternally(BuildInfo.donationPage)
  }
}
