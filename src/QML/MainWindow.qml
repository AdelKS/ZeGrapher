import QtQuick
import QtQuick.Controls
import zegrapher.interactivegraph 1.0

Item {
  id: win
  width: 800
  height: 600

  Drawer {
    id: drawer
    width: Math.max(0.3 * parent.width,
                    userInput.implicitWidth + 2*side_margins)
    height: parent.height
    edge: Qt.LeftEdge

    property int side_margins: 10

    UserInputPanel {
      id: userInput
      anchors.fill: parent
      anchors.margins: drawer.side_margins
    }
  }

  InteractiveGraphView {
    anchors.fill: parent
  }
}
