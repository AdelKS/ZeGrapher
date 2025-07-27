import QtQuick
import QtQuick.Controls

Window {
  id: win
  width: 800
  height: 600
  visible: true

  Button {
    id: drawer_button
    checkable: true
    checked: true
    z: 100
    x: 5
    y: 5
    width: 25
    height: 25
    flat: true
    icon.source: 'qrc:/icons/drawer.svg'
    icon.width: width
    icon.height: height
    display: Button.IconOnly
    padding: 0
  }

  Rectangle {
    id: drawer
    x: 0
    z: 50
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: row.width
    color: myPalette.window

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

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
      spacing: 10
      leftPadding: 10

      UserInputPanel {
        id: userInput
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: 250
      }

      Item {
        id: resizeHandle1
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
            if (drawer.width + diff < win.width) {
              userInput.width += diff;
            }
          }
        }

      }

      DataTable {
        id: dataTable
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: 50

        property int widthWhenVisible: 50
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
            if (drawer.width + diff < win.width && dataTable.width + diff > 0) {
              dataTable.widthWhenVisible = dataTable.width + diff;
              dataTable.width += diff;
            }
          }
        }
      }

      states: [
        State {
          name: "hidden";
          when: Information.dataColumnCount === 0
          PropertyChanges {
            dataTable.opacity: 0.
            resizeHandle2.opacity: 0.
            dataTable.visible: false
            resizeHandle2.visible: false
            dataTable.width: 0
            resizeHandle2: 0
          }
        },
        State {
          name: "shown";
          when: Information.dataColumnCount !== 0
          PropertyChanges {
            dataTable.opacity: 1.
            resizeHandle2.opacity: 1.
            dataTable.visible: true
            resizeHandle2.visible: true
            dataTable.width: dataTable.widthWhenVisible
            resizeHandle2: 5
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
  }

  InteractiveGraphView {
    id: interactiveGraph
    x: drawer.x + drawer.width
    y: 0
    height: win.height
    width: win.width - (drawer.x + drawer.width)

    states: State {
      name: "anchorToWindow"; when: win.width - (drawer.x + drawer.width) < 400
      PropertyChanges { interactiveGraph.x: 0; interactiveGraph.width: win.width }
    }

    transitions: Transition {
      NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
      NumberAnimation { properties: "width"; easing.type: Easing.InOutQuad }
    }

  }
}
