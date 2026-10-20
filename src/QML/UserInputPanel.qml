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

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
  id: userInputPanel
  clip: true

  signal exportGraph(file: url)

  required property ZeGraphSettings graphSettings

  property alias mathObjectsTab: mathObjectsTab

  /// @brief the size of the button tray that floats over the tabs
  readonly property size menuSize: Qt.size(globalMenu.width, globalMenu.height)

  FileDialog {
    id: exportDialog
    title: qsTr("Export the graph")
    currentFolder: Information.folder
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
    // an internal error has no file
    text: [currentError.file, currentError.text].filter((part) => part.length !== 0).join("\n\n")
    detailedText: currentError.details
    buttons: MessageDialog.Ok

    onAccepted: showNextIfAny()

    // main() reads the startup documents before it loads the QML, so the
    // Connections below never receives their ioErrorAdded()
    Component.onCompleted: Qt.callLater(ioErrorDialog.showNextIfAny)

    function showNextIfAny() {
      if (Information.hasIoError()) {
        currentError = Information.popIoError();
        open();
      }
    }
  }

  Connections {
    target: Information
    function onIoErrorAdded() {
      if (!ioErrorDialog.visible)
        ioErrorDialog.showNextIfAny()
    }
  }


  FileDialog {
    id: saveDialog
    title: qsTr("Save as a ZeGrapher document")
    currentFolder: Information.folder
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
    currentFolder: Information.folder
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
        globalMenuSize: userInputPanel.menuSize
      }

      GridSettings {
        id: gridSettings
        graphSettings: userInputPanel.graphSettings
        globalMenuSize: userInputPanel.menuSize
      }

      GraphSettings {
        id: graphSettingsTab
        graphSettings: userInputPanel.graphSettings
        globalMenuSize: userInputPanel.menuSize
      }

      AppSettings {
        id: appSettings
        globalMenuSize: userInputPanel.menuSize
      }

      About {
        id: aboutTab
        globalMenuSize: userInputPanel.menuSize
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

          onClicked: exportDialog.open()
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

          onClicked: userInputPanel.save()
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

          onClicked: loadDialog.open()
        }
      }
    }
  }
}
