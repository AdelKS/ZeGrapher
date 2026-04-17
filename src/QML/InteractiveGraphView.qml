import QtQuick.Controls.FluentWinUI3

ScrollView {
  id: scrollView

  property alias settings: interactiveGraph.settings

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent

    CurvesOverlay {
      x: interactiveGraph.graphRect.x
      y: interactiveGraph.graphRect.y
      width: interactiveGraph.graphRect.width
      height: interactiveGraph.graphRect.height
      qmlData: interactiveGraph.qmlData
    }
  }
}
