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
    nameFilters: [
      qsTr("Portable Document Format (%1)").arg("*.pdf"),
      qsTr("Scalable Vector Graphics (%1)").arg("*.svg"),
      qsTr("Image format (%1)").arg("*.png *.jpeg *.jpg *.bmp *.ppm") // supported formats: https://doc.qt.io/qt-6.11/qimage.html#reading-and-writing-image-files
    ]
    // MainWindow reads the format from the file suffix, so a name typed without
    // one gets the first extension of the selected filter. 'extensions' holds
    // them without the dot: '*.pdf' gives 'pdf'
    defaultSuffix: selectedNameFilter.extensions.length > 0
                   ? selectedNameFilter.extensions[0] : "pdf"
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

    // main() reads the startup documents before it loads the QML, so the
    // Connections below never receives their ioErrorCountChanged()
    Component.onCompleted: Qt.callLater(ioErrorDialog.showNextIfAny)

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
    onAccepted: Information.saveAs(selectedFile)
  }

  function save() {
    if (Information.documentName.length !== 0)
      Information.save();
    else
      saveDialog.open();
  }

  /// @brief a hint followed by a shortcut's keys, in the platform's notation
  function withKeys(hint: string, shortcut: Shortcut): string {
    return hint + " (" + shortcut.nativeText + ")";
  }

  Shortcut {
    id: exportShortcut
    sequence: "Ctrl+E"
    onActivated: exportDialog.open()
  }

  Shortcut {
    id: saveShortcut
    sequences: [StandardKey.Save]
    onActivated: userInputPanel.save()
  }

  // StandardKey.SaveAs has no keys on Windows
  Shortcut {
    sequence: "Ctrl+Shift+S"
    onActivated: saveDialog.open()
  }

  Shortcut {
    id: loadShortcut
    sequences: [StandardKey.Open]
    onActivated: loadDialog.open()
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

  ButtonTray {
    id: globalMenu

    anchors.left: userInputPanel.left
    anchors.leftMargin: 10

    anchors.bottom: userInputPanel.bottom
    anchors.bottomMargin: 10

    height: implicitHeight
    width: implicitWidth

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

          ToolTip.text: userInputPanel.withKeys(
            qsTr("Export the graph: vector graphics (PDF, SVG) or images (PNG, JPG)"), exportShortcut)

          onReleased: exportDialog.open()
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: saveDocument
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          iconName: "save"

          ToolTip.text: userInputPanel.withKeys(
            Information.documentName.length !== 0
              //: %1 is the name of the file of the document
              ? qsTr("Save to %1").arg(Information.documentName)
              : qsTr("Save as a ZeGrapher document"),
            saveShortcut)

          onReleased: userInputPanel.save()
        }

        IconRoundButton {
          Layout.topMargin: 0
          Layout.bottomMargin: 0

          id: loadDocument
          implicitWidth: 35
          implicitHeight: 35
          Layout.alignment: Qt.AlignRight

          iconName: "load"

          ToolTip.text: userInputPanel.withKeys(qsTr("Load a ZeGrapher document"), loadShortcut)

          onReleased: loadDialog.open()
        }
      }
    }
  }
}
