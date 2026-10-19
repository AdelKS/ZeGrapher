/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

RowLayout {
  id: root

  /// @brief the images to pick from: {icon}, an icon from icons/ in the app's
  ///        theme, or {} for a blank entry
  property alias model: tumbler.model
  property int fillMode: Image.PreserveAspectFit
  property alias currentIndex: tumbler.currentIndex

  Image {
    id: selector1
    Layout.alignment: Qt.AlignVCenter
    source: ZeStyle.icon("selector")
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }

  Tumbler {
    id: tumbler
    enabled: root.enabled
    wrap: true

    Layout.fillHeight: true
    Layout.fillWidth: true

    visibleItemCount: 3

    delegate: Image {
      required property var modelData

      source: modelData.icon ? ZeStyle.icon(modelData.icon) : ""
      opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * (0.1 + 0.5 * enabled)
      fillMode: root.fillMode
    }
  }

  Image {
    id: selector2
    Layout.alignment: Qt.AlignVCenter
    source: ZeStyle.icon("selector-flipped")
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }
}
