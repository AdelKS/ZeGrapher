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

  FileDialog {
    id: exportDialog
    title: qsTr("Export the graph")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.SaveFile
    defaultSuffix: "pdf"
    nameFilters: [
      qsTr("Portable Document Format (%1)").arg("*.pdf"),
      qsTr("Scalable Vector Graphics (%1)").arg("*.svg"),
      qsTr("Image format (%1)").arg("*.png *.jpeg *.jpg *.bmp *.ppm") // supported formats: https://doc.qt.io/qt-6.11/qimage.html#reading-and-writing-image-files
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
    title: qsTr("Save as a ZeGrapher document")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.SaveFile
    defaultSuffix: ".zg"
    nameFilters: [
      qsTr("ZeGrapher document (%1)").arg("*.zg"),
    ]
    visible: false
    onAccepted: Information.exportYaml(selectedFile)
  }

  FileDialog {
    id: loadDialog
    title: qsTr("Load a ZeGrapher document")
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    defaultSuffix: ".zg"
    nameFilters: [
      qsTr("ZeGrapher document (%1)").arg("*.zg"),
    ]
    visible: false
    onAccepted: Information.importYaml(selectedFile)
  }

  ColumnLayout {
    anchors.fill: parent

    RangeAdjust {
      id: rangeAdjust
      Layout.topMargin: 10
      Layout.fillWidth: true
      Layout.bottomMargin: 10
      range: userInputPanel.graphSettings.range
    }

    ZeTabBar {
      id: bar
      Layout.fillWidth: true

      onCurrentIndexChanged: swipeView.setCurrentIndex(currentIndex)

      ZeTabButton {
        //: title of the tab that holds the math objects. The screenshots of the
        //: website cut the row of tabs at the same width in every language, so
        //: keep the title short. A word such as 'Objects' is fine
        text: qsTr("Math")
      }

      ZeTabButton {
        text: qsTr("Grid")
      }

      ZeTabButton {
        text: qsTr("Graph")
      }

      ZeTabButton {
        //: title of the tab that holds the settings of the app, keep it short
        text: qsTr("App")
      }

      ZeTabButton {
        //: title of the tab that tells what the app is, keep it short
        text: qsTr("About")
      }
    }

    SwipeView {
      id: swipeView

      Layout.fillWidth: true
      Layout.fillHeight: true

      onCurrentIndexChanged: bar.setCurrentIndex(currentIndex)

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

      About {
        id: aboutTab
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

    background: Rectangle {
      color: ZeStyle.palette.light
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

          iconName: "export-graph"

          ToolTip.text: qsTr("Export the graph: vector graphics (PDF, SVG) or images (PNG, JPG)")

          onReleased: exportDialog.visible = true;
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: saveWorkspace
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          iconName: "save"

          ToolTip.text: qsTr("Save as a ZeGrapher document")

          onReleased: saveDialog.visible = true;
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: loadWorkspace
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          iconName: "load"

          ToolTip.text: qsTr("Load a ZeGrapher document")

          onReleased: loadDialog.visible = true;
        }
      }
    }
  }
}
