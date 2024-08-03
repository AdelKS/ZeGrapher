import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
  id: root

  property alias model: tumbler.model
  property real maxWidth: Number.POSITIVE_INFINITY
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
    Layout.maximumWidth: root.maxWidth
    visibleItemCount: 3
    delegate: Label {
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      height: contentHeight
      id: txtDelegate
      text: txt
      opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * (0.1 + 0.5 * enabled)
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
