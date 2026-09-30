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

      onReleased: {
        zoom.increase();
      }

      iconName: "zoom-in-icon"
    }

    ZeDoubleSpinBox {
      id: zoom
      Layout.maximumHeight: root.iconSize

      suffix: " %"
      from: 5
      to: 1000
      step: 10
      decimals: 2

      value: root.graphSettings.zoom.zoom * 100

      onValueModified: (value) => {
        root.graphSettings.zoom.zoomingType = ZoomingType.CUSTOM;
        root.graphSettings.zoom.zoom = value / 100.0;
      }
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      onReleased: {
        zoom.decrease();
      }

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

      iconName: "size"
    }
  }

  bottomRightRadius: 5
  bottomLeftRadius: 5

}
