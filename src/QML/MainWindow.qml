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
    icon.source: 'qrc:/icons/drawer.svg'
    icon.width: width
    icon.height: height
    display: Button.IconOnly
    padding: 0
  }

  Rectangle {
    id: drawer
    x: 0
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 200
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

    UserInputPanel {
      id: userInput
      anchors.top: drawer.top
      anchors.bottom: drawer.bottom
      anchors.left: drawer.left
      anchors.right: drawer.right

      anchors.rightMargin: resizeArea.width
      anchors.leftMargin: 10
      width: parent.width - resizeArea.width

      property int minWidth: implicitWidth
    }

    MouseArea {
      id: resizeArea
      z: 100
      width: 10
      anchors.top: drawer.top
      anchors.bottom: drawer.bottom
      anchors.left: userInput.right
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
  }

  InteractiveGraphView {
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.right: parent.right
    anchors.left: drawer.right
  }
}
