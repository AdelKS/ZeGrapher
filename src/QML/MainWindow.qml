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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Effects
import QtQuick.Window

ApplicationWindow {
  id: win
  // a size saved on a larger screen
  width: Math.min(Information.appSettings.windowSize.width, Screen.desktopAvailableWidth)
  height: Math.min(Information.appSettings.windowSize.height, Screen.desktopAvailableHeight)
  visibility: Information.appSettings.maximized ? Window.Maximized : Window.Windowed
  font: Information.appSettings.font

  title: Information.documentName.length !== 0 ? Information.documentName + " — ZeGrapher" : "ZeGrapher"

  color: ZeStyle.themed(Information.graphSettings.backgroundColor)

  // a right-to-left language mirrors the window, popups included, and puts the
  // panels on the right. The graph is not mirrored
  readonly property bool rtl: Application.layoutDirection === Qt.RightToLeft
  LayoutMirroring.enabled: rtl
  LayoutMirroring.childrenInherit: true

  /// @brief the x that puts an item at distance 'x' from the reading start:
  ///        from the left edge in left-to-right, from the right edge in
  ///        right-to-left
  /// @note LayoutMirroring does not change the coordinate system: still left
  ///       to right, with the zero on the left. So the panes set their x
  ///       through this. It also works the other way: given an item's x, it
  ///       returns its distance from the reading start.
  function flipX(x: real, width: real): real {
    return rtl ? win.width - x - width : x;
  }

  component ResizeHandle: Item {
    width: 5
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    z: 100

    signal dragStarted()

    /// @brief horizontal distance dragged since the press, towards the end of the
    ///        reading direction
    signal dragged(real diff)

    MouseArea {
      id: handleArea
      anchors.fill: parent

      hoverEnabled: true
      cursorShape: Qt.SizeHorCursor
      acceptedButtons: Qt.LeftButton

      ToolTip.delay: ZeStyle.tooltipDelay
      ToolTip.text: qsTr("Drag to move the border")
      ToolTip.visible: containsMouse && !pressed

      property real sceneXonPress

      onPressed: function (mouse) {
        // mapToItem(null) maps to window coordinates, with (0, 0) at the top left corner
        sceneXonPress = mapToItem(null, mouse.x, 0).x;
        parent.dragStarted();
      }

      onPositionChanged: function (mouse) {
        if (pressed)
          parent.dragged((Application.layoutDirection === Qt.RightToLeft ? -1 : 1)
                         * (mapToItem(null, mouse.x, 0).x - sceneXonPress));
      }

      Rectangle {
        anchors.centerIn: parent
        width: 3
        height: parent.height / 2
        radius: width / 2
        color: handleArea.pressed || handleArea.containsMouse ? ZeStyle.palette.highlight : ZeStyle.palette.mid
      }
    }
  }

  // a maximized window keeps its previous size. The platform can report the new
  // size before the new visibility, so read the size a moment later
  function saveSize() {
    if (win.visibility === Window.Windowed)
      Information.appSettings.windowSize = Qt.size(width, height);
  }

  onWidthChanged: Qt.callLater(saveSize)
  onHeightChanged: Qt.callLater(saveSize)

  onVisibilityChanged: (visibility) => {
    if (visibility === Window.Windowed || visibility === Window.Maximized)
      Information.appSettings.maximized = visibility === Window.Maximized;
  }

  onScreenChanged: {
    Information.graphSettings.screenChanged(win);
  }

  Connections {
    target: win
    enabled: AnimationConductor.animating

    // afterAnimating fires once per frame, so the steps follow the screen refresh rate
    function onAfterAnimating() {
      AnimationConductor.animationStep();
    }
  }

  Connections {
    target: AnimationConductor
    function onAnimatingChanged() {
      if (AnimationConductor.animating) AnimationConductor.animationStep()
    }
  }

  RectangularShadow {
    z: 49
    anchors.fill: drawer
    radius: drawer.radius
    blur: 10
    spread: 0
    offset.x: 0
    color: ZeStyle.palette.shadow
    opacity: drawer.width > 0 ? Math.max(0, 1 + win.flipX(drawer.x, drawer.width) / drawer.width) : 1
  }

  Rectangle {
    id: drawer
    x: 0
    z: 50
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: row.width

    color: ZeStyle.palette.window

    states: [
      State {
        name: "hidden"; when: !drawer_button.checked
        PropertyChanges { drawer.x: win.flipX(-drawer.width, drawer.width) }
      },
      State {
        name: "shown"; when: drawer_button.checked
        PropertyChanges { drawer.x: win.flipX(0, drawer.width) }
      }
    ]

    transitions: Transition {
      reversible: true
      from: "hidden"
      to: "shown"
      NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }

    Row {
      id: row
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.left: parent.left

      Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 10
      }

      UserInputPanel {
        id: userInput
        graphSettings: interactiveGraph.settings
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: Information.appSettings.paneWidth

        property int maxWidth: win.width - dataPane.width - 50;

        Connections {
          target: userInput.mathObjectsTab

          function onImportCSV(file: url, dataSheet: DataSheet)
          {
            dataPane.loadCSV(file, dataSheet);
          }
        }

        onExportGraph: (file) => interactiveGraph.exportTo(file)
      }

      Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 5
      }

      ResizeHandle {
        id: resizeHandle1

        property int widthOnPress: 0

        onDragStarted: widthOnPress = userInput.width

        onDragged: function (diff) {
          Information.appSettings.paneWidth = Math.min(Math.max(widthOnPress + diff, 100), userInput.maxWidth);
        }
      }

      DataPane {
        id: dataPane

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10

        property int resizedWidthDiff: 0
        property int maxWidth: win.width - userInput.width - 50;
      }

      ResizeHandle {
        id: resizeHandle2

        property int diffOnPress: 0

        onDragStarted: diffOnPress = dataPane.resizedWidthDiff

        onDragged: function (diff) {
          dataPane.resizedWidthDiff = diffOnPress + diff;

        }
      }

      states: [
        State {
          name: "hidden";
          when: DataTableModel.columns === 0 && !dataPane.importingCSV
          PropertyChanges {
            dataPane.opacity: 0.
            resizeHandle2.opacity: 0.
            dataPane.visible: false
            resizeHandle2.visible: false
            dataPane.width: 0
            resizeHandle2.width: 0
          }
        },
        State {
          name: "shown";
          when: DataTableModel.columns !== 0 || dataPane.importingCSV
          PropertyChanges {
            dataPane.opacity: 1.
            resizeHandle2.opacity: 1.
            dataPane.visible: true
            resizeHandle2.visible: true
            dataPane.width: Math.min(Math.max(dataPane.implicitWidth + dataPane.resizedWidthDiff, resizeHandle2.width), dataPane.maxWidth)
            resizeHandle2.width: 5
          }
        }
      ]

      transitions: Transition {
        reversible: true
        from: "shown"
        to: "hidden"
        SequentialAnimation {
          ParallelAnimation {
            NumberAnimation {
              easing.type: Easing.InOutQuad;
              property: "opacity";
              duration: 250;
            }
            NumberAnimation {
              easing.type: Easing.InOutQuad;
              property: "width";
              duration: 250;
            }
          }
          PropertyAction {
            property: "visible"
          }
        }
      }
    }

    EdgeButton {
      id: drawer_button
      z: +1

      checked: true

      anchors.top: parent.top
      anchors.topMargin: 8

      ToolTip.text: drawer_button.checked ? qsTr("Fold the panel away") : qsTr("Unfold the panel")

      Image {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: parent.checked ? 2 : 3.
        width: 10

        rotation: drawer_button.checked ? 180 : 0
        mirror: win.rtl

        Behavior on rotation {
          NumberAnimation { duration: 1000; easing.type: Easing.InOutQuad }
        }

        source: ZeStyle.icon("selector")
        fillMode: Image.PreserveAspectFit
        mipmap: true

        id: arrow
      }
    }
  }

  ScrollView {
    property bool anchorToDrawer: drawer_button.checked && win.width - drawer.width >= 400
    id: graphScrollView

    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    x: win.flipX(drawer_button.apparentWidth + (anchorToDrawer ? drawer.width : 0), width)
    y: 0
    height: win.height
    width: (anchorToDrawer ? win.width - drawer.width : win.width) - drawer_button.apparentWidth

    contentHeight: interactiveGraph.implicitHeight
    contentWidth: interactiveGraph.implicitWidth

    Binding {
      target: interactiveGraph.settings
      property: "availableSizePx"
      value: Qt.size(graphScrollView.availableWidth, graphScrollView.availableHeight)
    }

    Behavior on x {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    Behavior on width {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    InteractiveGraph {
      id: interactiveGraph
      anchors.centerIn: parent

      readonly property bool fitsView: settings.size.sheetFillsWindow
                                       || settings.zoom.zoomingType === ZoomingType.FITSHEET
      readonly property real pxPerCm: settings.pixelDensity * settings.zoom.zoom

      implicitWidth: fitsView ? graphScrollView.availableWidth : settings.size.cmSheetSize.width * pxPerCm
      implicitHeight: fitsView ? graphScrollView.availableHeight : settings.size.cmSheetSize.height * pxPerCm

      width: implicitWidth
      height: implicitHeight

      // a pinch on a touchscreen or a touchpad zooms the view
      PinchHandler {
        target: null
        onScaleChanged: (delta) => interactiveGraph.zoomAt(centroid.position, delta)
      }
    }
  }

  RectangularShadow {
    // drawn over the graph, under the pane it belongs to, and under the panel
    z: 39
    anchors.fill: docPane
    radius: docPane.radius
    blur: 10
    spread: 0
    offset.x: 0
    color: ZeStyle.palette.shadow
    opacity: docPane.width > 0 ? Math.max(0, 1 + (win.flipX(docPane.x, docPane.width) - win.docsEdge) / docPane.width) : 1
  }

  /// @brief the panel's inner edge, folded or not, from the reading start. The
  ///        documentation pane unfolds from there
  readonly property real docsEdge: win.flipX(drawer.x, drawer.width) + drawer.width

  DocPane {
    id: docPane

    z: 40
    y: 0
    height: win.height
    width: Math.min(700, win.width - win.docsEdge - 15)

    // its button sits under the panel's button, and follows the pane's edge the
    // same way. The panel is drawn over it, so the button slides out from behind
    // the panel as the pane unfolds
    buttonTopMargin: drawer_button.anchors.topMargin + drawer_button.height + 8

    states: [
      State {
        name: "hidden"; when: !docPane.open
        PropertyChanges { docPane.x: win.flipX(win.docsEdge - docPane.width, docPane.width) }
      },
      State {
        name: "shown"; when: docPane.open
        PropertyChanges { docPane.x: win.flipX(win.docsEdge, docPane.width) }
      }
    ]

    transitions: Transition {
      reversible: true
      from: "hidden"
      to: "shown"
      NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }
  }

  ZoomSettings {
    id: zoomSettings
    y: 0
    graphSettings: interactiveGraph.settings

    anchors.horizontalCenter: graphScrollView.horizontalCenter

    states: [
      State {
        name: "hidden";
        when: interactiveGraph.settings.size.sheetFillsWindow
        PropertyChanges {
          zoomSettings.y: -zoomSettings.height;
          zoomSettings.visible: false;
        }
      },
      State {
        name: "shown";
        when: !interactiveGraph.settings.size.sheetFillsWindow
        PropertyChanges {
          zoomSettings.y: 0;
          zoomSettings.visible: true;
        }
      }
    ]

    transitions: Transition {
      reversible: true
      from: "shown"
      to: "hidden"
      SequentialAnimation {
        NumberAnimation {
          easing.type: Easing.InOutQuad;
          property: "y";
          duration: 250;
        }
        PropertyAction {
          property: "visible"
        }
      }
    }
  }

  UpdateBanner {
    id: updateBanner

    // over the graph, under the panes
    z: 30
    width: Math.min(implicitWidth, graphScrollView.width - 20)
    anchors.horizontalCenter: graphScrollView.horizontalCenter

    states: [
      State {
        name: "hidden"; when: !updateBanner.shown
        PropertyChanges { updateBanner.y: win.height; updateBanner.visible: false }
      },
      State {
        name: "shown"; when: updateBanner.shown
        PropertyChanges { updateBanner.y: win.height - updateBanner.height - 16; updateBanner.visible: true }
      }
    ]

    transitions: Transition {
      reversible: true
      from: "hidden"
      to: "shown"
      SequentialAnimation {
        PropertyAction { property: "visible" }
        NumberAnimation { property: "y"; duration: 250; easing.type: Easing.InOutQuad }
      }
    }
  }

  ReleaseNotesDialog {
    id: releaseNotesDialog
  }

  // a document or a CSV file dropped on the window loads as if picked in a dialog
  DropArea {
    id: dropArea
    anchors.fill: parent

    function suffix(url: url): string {
      return url.toString().toLowerCase().split(".").pop();
    }

    onEntered: (drag) => drag.accepted = drag.hasUrls
                         && drag.urls.some((url) => ["zg", "csv"].includes(dropArea.suffix(url)))

    onDropped: (drop) => {
      for (const url of drop.urls)
        if (dropArea.suffix(url) === "zg")
          Information.importYaml(url);

      // the import settings take one file at a time
      const csv = drop.urls.find((url) => dropArea.suffix(url) === "csv");
      if (csv)
        dataPane.loadCSV(csv, null);

      drop.accept(Qt.CopyAction);
    }
  }

  Component.onCompleted: {
    Information.graphSettings.screenChanged(win);

    if (Information.appSettings.checkUpdates)
      Information.updateCheck.refresh();

    // the settings file holds the version that ran last, and the app writes
    // it on close, so the dialog opens once per update
    if (releaseNotesDialog.entries.length !== 0)
      releaseNotesDialog.open();
  }
}
