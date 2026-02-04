// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Rectangle {
  id: root

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

      lightThemeIcon: 'qrc:/icons/zoom-in-icon.svg'
      darkThemeIcon: 'qrc:/icons/zoom-in-icon-light.svg'
    }

    ZeDoubleSpinBox {
      id: zoom
      Layout.maximumHeight: root.iconSize

      suffix: " %"
      from: 5
      to: 1000
      value: Information.graphZoomSettings.zoom * 100;
      step: 10
      decimals: 2

      Connections {
        target: Information

        function onGraphZoomSettingsChanged() {
          zoom.value = Information.graphZoomSettings.zoom * 100;
        }

        enabled: !Information.graphSizeSettings.sheetFillsWindow
      }

      onValueModified: {
        fitSheet.checked = false;
        Information.graphZoomSettings.zoom = value / 100.0;
      }
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      lightThemeIcon: 'qrc:/icons/zoom-out-icon.svg'
      darkThemeIcon: 'qrc:/icons/zoom-out-icon-light.svg'
    }

    IconButton {
      Layout.maximumHeight: root.iconSize
      Layout.maximumWidth: root.iconSize

      id: fitSheet

      onCheckedChanged: {
        Information.graphZoomSettings.zoomingType = checked ? ZoomingType.FITSHEET : ZoomingType.CUSTOM;
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
