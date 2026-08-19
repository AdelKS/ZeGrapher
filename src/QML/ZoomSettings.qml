// The zoom buttons and the zoom value, in a bar that floats over the graph

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Rectangle {
  id: root

  required property ZeGraphSettings graphSettings

  readonly property int margin: 10
  readonly property int iconSize: Math.max(30, zoom.implicitHeight)

  color: myPalette.window

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

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

      ToolTip.text: qsTr("Zoom in")

      lightThemeIcon: 'qrc:/icons/zoom-in-icon.svg'
      darkThemeIcon: 'qrc:/icons/zoom-in-icon-light.svg'
    }

    ZeDoubleSpinBox {
      id: zoom
      Layout.maximumHeight: root.iconSize

      suffix: " %"
      from: 5
      to: 1000
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

      onReleased: {
        zoom.decrease();
      }

      ToolTip.text: qsTr("Zoom out")

      lightThemeIcon: 'qrc:/icons/zoom-out-icon.svg'
      darkThemeIcon: 'qrc:/icons/zoom-out-icon-light.svg'
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

      lightThemeIcon: 'qrc:/icons/size.svg'
      darkThemeIcon: 'qrc:/icons/size-light.svg'
    }
  }

  bottomRightRadius: 5
  bottomLeftRadius: 5

}
