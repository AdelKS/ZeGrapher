import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
  id: win
  width: Information.appSettings.windowSize.width
  height: Information.appSettings.windowSize.height
  visible: true
  font: Information.appSettings.font

  color: Information.graphSettings.backgroundColor.current

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  component ResizeHandle: Item {
    width: 5
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    z: 100

    signal dragStarted()

    /// @brief horizontal distance dragged since the press, in window coordinates
    signal dragged(real diff)

    MouseArea {
      id: handleArea
      anchors.fill: parent

      hoverEnabled: true
      cursorShape: Qt.SizeHorCursor
      acceptedButtons: Qt.LeftButton

      property real sceneXonPress

      onPressed: function (mouse) {
        // mapToItem(null) maps to the window, i.e. (0,0) is top left corner of it
        sceneXonPress = mapToItem(null, mouse.x, 0).x;
        parent.dragStarted();
      }

      onPositionChanged: function (mouse) {
        if (pressed)
          parent.dragged(mapToItem(null, mouse.x, 0).x - sceneXonPress);
      }

      Rectangle {
        anchors.centerIn: parent
        width: 3
        height: parent.height / 2
        radius: width / 2
        color: handleArea.pressed || handleArea.containsMouse ? myPalette.highlight : myPalette.mid
      }
    }
  }

  onWidthChanged: Information.appSettings.windowSize = Qt.size(width, height)

  onHeightChanged: Information.appSettings.windowSize = Qt.size(width, height)

  onScreenChanged: {
    console.log("Moved to screen:", screen.name);
    Information.graphSettings.screenChanged(win); // converted from (px per mm) to (px per cm)
  }

  Connections {
    target: win
    enabled: AnimationConductor.animating

    // naturally follows the refresh rate of the screen
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
    color: myPalette.shadow
    opacity: drawer.width > 0 ? Math.max(0, 1 + drawer.x / drawer.width) : 1
  }

  Rectangle {
    id: drawer
    x: 0
    z: 50
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: row.width

    color: myPalette.window

    states: [
      State {
        name: "hidden"; when: !drawer_button.checked
        PropertyChanges { drawer.x: -drawer.width }
      },
      State {
        name: "shown"; when: drawer_button.checked
        PropertyChanges { drawer.x: 0 }
      }
    ]

    transitions: Transition {
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

        onExportGraph: function (file: url)
        {
          if (file.toString().toLowerCase().endsWith(".pdf"))
            interactiveGraph.graph.exportPDF(file);
          else if (file.toString().toLowerCase().endsWith(".svg"))
            interactiveGraph.graph.exportSVG(file);
          else interactiveGraph.graph.exportImage(file);
        }
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

    RectangularShadow {
      z: -2
      anchors.fill: drawer_button
      radius: drawer_button.radius
      blur: 10
      spread: 0
      color: myPalette.shadow
    }

    Rectangle {
      id: drawer_button
      width: 25
      height: width
      radius: 8
      color: myPalette.window
      z: +1

      property int apparentWidth: 2.*width/3.
      property bool checked: true

      anchors.top: parent.top
      anchors.topMargin: 8
      anchors.right: parent.right
      anchors.rightMargin: -apparentWidth

      Image {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: parent.checked ? 2 : 3.
        width: 10

        rotation: drawer_button.checked ? 180 : 0

        Behavior on rotation {
          NumberAnimation { duration: 1000; easing.type: Easing.InOutQuad }
        }

        source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/selector.svg" : "qrc:/icons/selector-light.svg"
        fillMode: Image.PreserveAspectFit
        mipmap: true

        id: arrow
      }

      MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: drawer_button.checked = !drawer_button.checked
      }
    }
  }

  ScrollView {
    property bool anchorToDrawer: drawer_button.checked && win.width - drawer.width >= 400
    id: graphScrollView

    x: drawer_button.apparentWidth + (anchorToDrawer ? drawer.width : 0)
    y: 0
    height: win.height
    width: (anchorToDrawer ? win.width - drawer.width : win.width) - drawer_button.apparentWidth

    contentHeight: interactiveGraph.implicitHeight
    contentWidth: interactiveGraph.implicitWidth

    onContentHeightChanged: {
      console.log("Graph scrollview content height: ", contentHeight);
    }

    onContentWidthChanged: {
      console.log("Graph scrollview content width: ", contentWidth);
    }

    onAvailableHeightChanged: {
      interactiveGraph.settings.setAvailableSizePx( Qt.size(availableWidth, availableHeight));
      interactiveGraph.updateImplicitSize();
      console.log("Graph available height: ", availableHeight);
    }

    onAvailableWidthChanged: {
      interactiveGraph.settings.setAvailableSizePx( Qt.size(availableWidth, availableHeight));
      interactiveGraph.updateImplicitSize();
      console.log("Graph available width: ", availableWidth);
    }

    Behavior on x {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    Behavior on width {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    InteractiveGraphView {
      id: interactiveGraph
      anchors.centerIn: parent

      width: implicitWidth
      height: implicitHeight

      hoverEnabled: true

      Connections {
        target: interactiveGraph.settings

        function onZoomSettingsChanged() {
          console.log("zoom settings change: updating implicit sizes");
          interactiveGraph.updateImplicitSize();
        }

        function onSizeSettingsChanged() {
          console.log("size settings change: updating implicit sizes");
          interactiveGraph.updateImplicitSize();
        }
      }

      onHeightChanged: {
        console.log("Graph width: ", width);
        console.log("Graph height: ", height);
      }

      onWidthChanged: {
        console.log("Graph width: ", width);
        console.log("Graph height: ", height);
      }

      function updateImplicitSize() {
        console.log("Updating graph implicit size")
        if (interactiveGraph.settings.size.sheetFillsWindow || interactiveGraph.settings.zoom.zoomingType === ZoomingType.FITSHEET ) {
          implicitWidth = graphScrollView.availableWidth;
          implicitHeight = graphScrollView.availableHeight;
        } else {
          implicitWidth = interactiveGraph.settings.size.pxSheetSize.width * interactiveGraph.settings.zoom.zoom;
          implicitHeight = interactiveGraph.settings.size.pxSheetSize.height * interactiveGraph.settings.zoom.zoom;
        }
      }
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

  Component.onCompleted: {
    Information.graphSettings.screenChanged(win);
  }
}
