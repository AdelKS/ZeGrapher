// A line edit where the user enters an expression that evaluates to a value

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

      Connections {
        target: root.graphSettings

        function onZoomChanged() {
          console.debug("Syncing zoom widget with backend");
          zoom.setValue(root.graphSettings.zoom.zoom * 100);
          fitSheet.checked = (root.graphSettings.zoom.zoomingType === ZoomingType.FITSHEET);
        }
      }

      onValueModified: {
        fitSheet.checked = false;
        graphSettings.zoom.zoom = value / 100.0;
      }

      Component.onCompleted: {
        setValue(graphSettings.zoom.zoom * 100);
      }
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      onReleased: {
        zoom.decrease();
      }

      lightThemeIcon: 'qrc:/icons/zoom-out-icon.svg'
      darkThemeIcon: 'qrc:/icons/zoom-out-icon-light.svg'
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      id: fitSheet

      onCheckedChanged: {
        graphSettings.zoom.zoomingType = checked ? ZoomingType.FITSHEET : ZoomingType.CUSTOM;
        Information.computeZoom();
      }

      checkable: true
      checked: true
      lightThemeIcon: 'qrc:/icons/size.svg'
      darkThemeIcon: 'qrc:/icons/size-light.svg'
    }
  }

  bottomRightRadius: 5
  bottomLeftRadius: 5

}
