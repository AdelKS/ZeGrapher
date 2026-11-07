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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// The notice that a newer release is out, which floats over the graph

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Effects
import QtQuick.Layouts

Item {
  id: root

  /// @brief set when the user closes the banner. It shows again at the next start
  property bool dismissed: false

  readonly property bool shown: !dismissed
                                && Information.updateCheck.status === UpdateCheck.UPDATE_AVAILABLE

  readonly property int margin: 10

  implicitWidth: layout.implicitWidth + 2*margin
  implicitHeight: layout.implicitHeight + 2*margin

  RectangularShadow {
    anchors.fill: card
    radius: card.radius
    blur: 10
    spread: 0
    color: ZeStyle.palette.shadow
  }

  Rectangle {
    id: card
    anchors.fill: parent
    radius: 8
    color: ZeStyle.palette.window

    RowLayout {
      id: layout
      anchors.fill: parent
      anchors.margins: root.margin
      spacing: 10

      Image {
        Layout.alignment: Qt.AlignVCenter
        source: "qrc:/icons/ZeGrapher.svg"
        sourceSize: Qt.size(24, 24)
        fillMode: Image.PreserveAspectFit
        mipmap: true
      }

      LinkLabel {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter

        //: %1 is a version, such as 4.1.0, and %2 is a link to the site, zegrapher.com
        markdown: qsTr("ZeGrapher %1 is available on %2.")
                    // the site gives a tag, 'v4.1.0'
                    .arg(Information.updateCheck.latestVersion.replace(/^v/, ""))
                    .arg("[" + new URL(BuildInfo.homepage).host + "](" + BuildInfo.homepage + ")")
      }

      ToolButton {
        Layout.alignment: Qt.AlignVCenter
        text: "✕"

        ToolTip.text: qsTr("Close")
        ToolTip.delay: ZeStyle.tooltipDelay
        ToolTip.visible: hovered
        Accessible.name: ToolTip.text

        onClicked: root.dismissed = true
      }
    }
  }
}
