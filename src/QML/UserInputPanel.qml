import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

Item {
  id: userInputPanel
  clip: true

  signal exportGraph(file: string)

  required property ZeGraphSettings graphSettings

  property alias mathObjectsTab: mathObjectsTab

  implicitWidth: colLayout.implicitWidth

  onImplicitWidthChanged: {
    console.debug("UserInputPanel: implicitWidth: ", implicitWidth);
  }

  onImplicitHeightChanged: {
    console.debug("UserInputPanel: implicitHeight: ", implicitHeight);
  }


  FileDialog {
    id: fileDialog
    title: qsTr("Graph export")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.SaveFile
    defaultSuffix: "pdf"
    nameFilters: [
      "Portable Document Format (*.pdf)",
      "Scalable Vector Graphics (*.svg)",
      "Image format (*.png *.jpeg *.jpg *.bmp *.ppm)" // supported formats: https://doc.qt.io/qt-6.11/qimage.html#reading-and-writing-image-files
    ]
    visible: false
    onAccepted: userInputPanel.exportGraph(selectedFile)
  }

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    RangeAdjust {
      id: rangeAdjust
      Layout.topMargin: 10
      Layout.fillWidth: true
      Layout.bottomMargin: 10
      range: userInputPanel.graphSettings.range
    }

    TabBar {
      id: bar
      Layout.fillWidth: true

      onCurrentIndexChanged: swipeView.setCurrentIndex(currentIndex)

      implicitWidth: mathTabButton.implicitWidth + graphTabButton.implicitWidth + appTabButton.implicitWidth

      onImplicitWidthChanged: {
        console.log("UserInputPanel: TabBar: implicitWidth: ", implicitWidth);
      }

      ZeTabButton {
        id: mathTabButton
        text: qsTr("Math")
      }

      ZeTabButton {
        id: gridTabButton
        text: qsTr("Grid")
      }

      ZeTabButton {
        id: graphTabButton
        text: qsTr("Graph")
      }

      ZeTabButton {
        id: appTabButton
        text: qsTr("App")
      }
    }

    SwipeView {
      id: swipeView

      Layout.fillWidth: true
      Layout.fillHeight: true

      implicitWidth: Math.max(mathObjectsTab.implicitWidth, gridSettings.implicitWidth, graphSettingsTab.implicitWidth, appSettings.implicitWidth)

      onCurrentIndexChanged: bar.setCurrentIndex(currentIndex)

      onImplicitWidthChanged: {
        console.log("UserInputPanel: SwipeView: implicitWidth: ", implicitWidth);
      }

      MathObjects {
        id: mathObjectsTab
      }

      GridSettings {
        id: gridSettings
        graphSettings: userInputPanel.graphSettings
      }

      GraphSettings {
        id: graphSettingsTab
        graphSettings: userInputPanel.graphSettings
      }

      AppSettings {
        id: appSettings
      }
    }
  }

  ScrollView {
    z: 100

    anchors.left: userInputPanel.left
    anchors.leftMargin: 10

    anchors.bottom: userInputPanel.bottom
    anchors.bottomMargin: 10

    height: implicitHeight
    width: implicitWidth

    contentHeight: height
    contentWidth: availableWidth

    implicitHeight: globalButtonsRow.implicitHeight
    implicitWidth: globalButtonsRow.implicitWidth

    ScrollBar.horizontal.visible: false
    ScrollBar.vertical.visible: false

    RowLayout
    {
      id: globalButtonsRow
      spacing: 5

      Item {
        Layout.fillWidth: true
      }

      IconRoundButton {
        Layout.topMargin: 0
        Layout.bottomMargin: 0
        id: exportGraph
        implicitWidth: 35
        implicitHeight: 35
        Layout.alignment: Qt.AlignRight

        lightThemeIcon: "qrc:/icons/export-graph-dark.svg"
        darkThemeIcon: "qrc:/icons/export-graph-light.svg"

        onReleased: fileDialog.visible = true;
      }
    }
  }
}
