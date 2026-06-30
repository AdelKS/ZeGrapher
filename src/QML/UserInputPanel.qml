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
    id: exportDialog
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

  MessageDialog {
    id: ioErrorDialog

    property ioError currentError

    title: currentError.title
    text: currentError.file + "\n\n" + currentError.text
    detailedText: currentError.details
    buttons: MessageDialog.Ok

    onAccepted: showNextIfAny()

    function showNextIfAny() {
      if (Information.ioErrorCount > 0) {
        currentError = Information.popIoError();
        open();
      }
    }
  }

  Connections {
    target: Information
    function onIoErrorCountChanged() {
      if (!ioErrorDialog.visible)
        ioErrorDialog.showNextIfAny()
    }
  }


  FileDialog {
    id: saveDialog
    title: qsTr("Save as ZeGrapher document")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.SaveFile
    defaultSuffix: ".zg"
    nameFilters: [
      "ZeGrapher document (*.zg)",
    ]
    visible: false
    onAccepted: Information.exportYaml(selectedFile)
  }

  FileDialog {
    id: loadDialog
    title: qsTr("Load ZeGrapher document")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    defaultSuffix: ".zg"
    nameFilters: [
      "ZeGrapher document (*.zg)",
    ]
    visible: false
    onAccepted: Information.importYaml(selectedFile)
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
        globalMenuSize: Qt.size(globalMenu.width, globalMenu.height)
      }

      GridSettings {
        id: gridSettings
        graphSettings: userInputPanel.graphSettings
        globalMenuSize: Qt.size(globalMenu.width, globalMenu.height)
      }

      GraphSettings {
        id: graphSettingsTab
        graphSettings: userInputPanel.graphSettings
        globalMenuSize: Qt.size(globalMenu.width, globalMenu.height)
      }

      AppSettings {
        id: appSettings
        globalMenuSize: Qt.size(globalMenu.width, globalMenu.height)
      }
    }
  }

  Frame {
    id: globalMenu

    z: 100

    leftPadding: 5
    rightPadding: 5
    topPadding: 5
    bottomPadding: 5

    anchors.left: userInputPanel.left
    anchors.leftMargin: 10

    anchors.bottom: userInputPanel.bottom
    anchors.bottomMargin: 10

    height: implicitHeight
    width: implicitWidth

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    background: Rectangle {
      color: myPalette.light
      radius: 5
      opacity: 0.95
    }

    ScrollView {
      contentWidth: availableWidth

      implicitHeight: globalButtonsRow.implicitHeight
      implicitWidth: globalButtonsRow.implicitWidth

      ScrollBar.horizontal.visible: false
      ScrollBar.vertical.visible: false

      RowLayout
      {
        id: globalButtonsRow
        spacing: 5

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0
          id: exportGraph
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          lightThemeIcon: "qrc:/icons/export-graph-dark.svg"
          darkThemeIcon: "qrc:/icons/export-graph-light.svg"

          onReleased: exportDialog.visible = true;
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: saveWorkspace
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          lightThemeIcon: "qrc:/icons/save-dark.svg"
          darkThemeIcon: "qrc:/icons/save-light.svg"

          onReleased: saveDialog.visible = true;
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: loadWorkspace
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          lightThemeIcon: "qrc:/icons/load-dark.svg"
          darkThemeIcon: "qrc:/icons/load-light.svg"

          onReleased: loadDialog.visible = true;
        }
      }
    }
  }
}
