import QtQuick.Controls.FluentWinUI3

ScrollView {
  id: scrollView

  property alias settings: interactiveGraph.settings

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent
  }
}
