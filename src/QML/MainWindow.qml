import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
  id: win
  width: 900
  height: 600
  visible: true
  font: Information.appSettings.font

  color: Information.graphSettings.backgroundColor.current

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  onWidthChanged: dataPane.updateWidthWhenVisible()

  onScreenChanged: {
    console.log("Moved to screen:", screen.name);
    Information.screenChanged(win); // converted from (px per mm) to (px per cm)
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

    color: myPalette.midlight

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

      Behavior on width {
        NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
      }

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

        Behavior on width {
          NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
        }

        Timer {
          id: widthResetTimer
          interval: 4000
          repeat: false
          onTriggered: userInput.width = userInput.implicitWidth;
        }

        Connections {
          target: userInput.mathObjectsTab

          function onImportCSV(file: url)
          {
            dataPane.loadCSV(file);
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

        Component.onCompleted: widthResetTimer.start()
      }

      Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 5
      }

      Item {
        id: resizeHandle1
        width: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 100

        MouseArea {
          anchors.fill: parent

          cursorShape: Qt.SizeHorCursor
          acceptedButtons: Qt.LeftButton

          property real mouseXonPress

          onPressed: {
            mouseXonPress = mouseX;
          }

          onMouseXChanged: {
            var diff = mouseX - mouseXonPress;
            if (drawer.width + diff < win.width) {
              userInput.width += diff;
            }
          }
        }

      }

      DataPane {
        id: dataPane

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10
        width: 150

        property int widthWhenVisible: 150

        Behavior on width {
          NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
        }

        function updateWidthWhenVisible() {
          if (implicitWidth > 0)
            widthWhenVisible = Math.min(implicitWidth, win.width - x);
        }

        onImplicitWidthChanged: updateWidthWhenVisible()
      }

      Item {
        id: resizeHandle2
        width: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 100

        ToolSeparator
        {
          orientation: Qt.Vertical
          anchors.fill: parent
        }

        MouseArea {
          anchors.fill: parent

          cursorShape: Qt.SizeHorCursor
          acceptedButtons: Qt.LeftButton

          property real mouseXonPress

          onPressed: {
            mouseXonPress = mouseX;
          }

          onMouseXChanged: {
            var diff = mouseX - mouseXonPress;
            if (drawer.width + diff < win.width && dataPane.width + diff > 0) {
              dataPane.widthWhenVisible = dataPane.width + diff;
              dataPane.width += diff;
            }
          }
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
            dataPane.width: dataPane.widthWhenVisible
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
      color: myPalette.midlight
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

        function onZoomChanged() {
          console.log("zoom settings change: updating implicit sizes");
          interactiveGraph.updateImplicitSize();
        }

        function onSizeChanged() {
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
    Information.screenChanged(win);
  }
}
