import QtQuick.Controls

ScrollView {
  id: scrollView

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent
  }
}
