import QtQuick.Controls.FluentWinUI3

ScrollView {
  id: scrollView

  property alias settings: interactiveGraph.settings
  property alias graph: interactiveGraph

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent
  }
}
