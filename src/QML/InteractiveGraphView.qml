import QtQuick 6.7
import QtQuick.Controls 6.7
import QtQuick.Layouts 6.7
import zegrapher.interactivegraph 1.0
import zegrapher.information 1.0

ScrollView {
  id: scrollView

  InteractiveGraph {
    width: scrollView.width
    height: scrollView.height

    id: interactiveGraph
    anchors.centerIn: parent
  }
}
