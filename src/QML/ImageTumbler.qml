import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

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

    Layout.minimumWidth: 0.5 * maxWidth
    Layout.preferredWidth: maxWidth
    Layout.maximumWidth: maxWidth
    Layout.fillWidth: true

    Layout.minimumHeight: 1.5 * maxHeight
    Layout.preferredHeight: 2.5 * maxHeight
    Layout.maximumHeight: 3 * maxHeight
    Layout.fillHeight: true

    visibleItemCount: 3

    property int maxHeight: 0
    property int maxWidth: 0

    delegate: Image {
      id: imgDelegate
      source: path
      opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * (0.1 + 0.5 * enabled)
      fillMode: root.fillMode

      onSourceSizeChanged: {
        if (tumbler.maxHeight < sourceSize.height)
          tumbler.maxHeight = sourceSize.height
        if (tumbler.maxWidth < sourceSize.width)
          tumbler.maxWidth = sourceSize.width
      }
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
