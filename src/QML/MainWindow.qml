import QtQuick
import QtQuick.Controls
import zegrapher.interactivegraph 1.0

Item {
  id: win
  width: 800
  height: 600

  Drawer {
    id: drawer
    width: 0.3 * parent.width
    height: parent.height
    edge: Qt.LeftEdge

    UserInputPanel {
      anchors.fill: parent
      anchors.margins: 20
    }
  }

  InteractiveGraphView {
    anchors.fill: parent
  }
}
