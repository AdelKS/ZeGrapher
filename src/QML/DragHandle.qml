// The handle to drag an object or column within its list

import QtQuick
import QtQuick.Layouts

Item {
  Layout.minimumWidth: 20
  Layout.maximumWidth: 30
  Layout.preferredHeight: Layout.preferredWidth
  Layout.preferredWidth: 30

  Image {
    anchors.fill: parent
    source: ZeStyle.icon("drag-handle")
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
