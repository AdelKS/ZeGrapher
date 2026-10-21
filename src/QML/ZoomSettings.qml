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

// The zoom buttons and the zoom value, in a bar that floats over the graph

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Rectangle {
  id: root

  required property ZeGraphSettings graphSettings

  readonly property int margin: 10
  readonly property int iconSize: Math.max(30, zoom.implicitHeight)

  color: ZeStyle.palette.window

  implicitHeight: layout.implicitHeight + 2*margin
  implicitWidth: layout.implicitWidth + 2*margin

  RowLayout {
    id: layout
    anchors.fill: parent
    anchors.leftMargin: root.margin
    anchors.rightMargin: root.margin
    anchors.topMargin: 3
    anchors.bottomMargin: 3
    spacing: 5

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      onClicked: {
        zoom.increase();
      }

      ToolTip.text: qsTr("Zoom in")

      iconName: "zoom-in-icon"
    }

    ZeDoubleSpinBox {
      id: zoom
      Layout.maximumHeight: root.iconSize

      suffix: " %"
      from: Limits.minZoom * 100
      to: Limits.maxZoom * 100
      step: 10
      decimals: 2

      ToolTip.text: qsTr("Zoom of the view")

      value: root.graphSettings.zoom.zoom * 100

      onValueModified: (value) => {
        root.graphSettings.zoom.zoomingType = ZoomingType.CUSTOM;
        root.graphSettings.zoom.zoom = value / 100.0;
      }
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      onClicked: {
        zoom.decrease();
      }

      ToolTip.text: qsTr("Zoom out")

      iconName: "zoom-out-icon"
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      id: fitSheet

      checkable: true
      checked: root.graphSettings.zoom.zoomingType === ZoomingType.FITSHEET

      onClicked: {
        root.graphSettings.zoom.zoomingType = checked ? ZoomingType.FITSHEET : ZoomingType.CUSTOM;
        root.graphSettings.computeZoom();
      }

      ToolTip.text: qsTr("Fit the graph to the window")

      iconName: "size"
    }
  }

  bottomRightRadius: 5
  bottomLeftRadius: 5

}
