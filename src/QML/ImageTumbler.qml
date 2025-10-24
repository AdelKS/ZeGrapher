import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

RowLayout {
  id: root

  property alias model: tumbler.model
  property int fillMode: Image.PreserveAspectFit
  property alias currentIndex: tumbler.currentIndex

  Image {
    id: selector1
    Layout.alignment: Layout.Center
    source: "qrc:/icons/selector.svg"
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }

  Tumbler {
    id: tumbler
    enabled: root.enabled
    wrap: true

    Layout.fillHeight: true
    Layout.fillWidth: true

    visibleItemCount: 3

    delegate: Image {
      id: imgDelegate
      source: path
      opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * (0.1 + 0.5 * enabled)
      fillMode: root.fillMode
    }
  }

  Image {
    id: selector2
    Layout.alignment: Layout.Center
    source: "qrc:/icons/selectorFlipped.svg"
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }
}
