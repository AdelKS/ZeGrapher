import QtQuick
import QtQuick.Controls
import zegrapher.interactivegraph 1.0

Item {
  id: win
  width: 800
  height: 600

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
    Image {
      width: 20
      height: 20
      anchors.centerIn: parent
      source: 'qrc:/icons/drawer.svg'
    }
  }

  Rectangle {
    id: drawer
    x: 0
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 200
    color: myPalette.window
    property int drawer_width: userInput.implicitWidth + 2*side_margins
    property int side_margins: 10

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

    MouseArea {
      width: drawer.side_margins
      anchors.right: drawer.right
      anchors.top: drawer.top
      anchors.bottom: drawer.bottom
      cursorShape: Qt.SizeHorCursor
      acceptedButtons: Qt.LeftButton

      property real mouseXonPress

      onPressed: {
        mouseXonPress = mouseX;
      }

      onMouseXChanged: {
        var diff = mouseX - mouseXonPress;
        if (drawer.width + diff > userInput.minWidth)
          drawer.width += diff;
      }
    }

    UserInputPanel {
      id: userInput
      anchors.fill: parent
      anchors.margins: drawer.side_margins

      property int minWidth: implicitWidth + 2*drawer.side_margins
    }
  }

  InteractiveGraphView {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.left: drawer.right
  }
}
