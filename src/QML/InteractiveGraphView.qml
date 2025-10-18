import QtQuick.Controls.FluentWinUI3

ScrollView {
  id: scrollView

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent
  }
}
