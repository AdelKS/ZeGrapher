// The handle to drag an object or column within its list

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  Layout.minimumWidth: 20
  Layout.maximumWidth: 30
  Layout.preferredHeight: Layout.preferredWidth
  Layout.preferredWidth: 30

  // call sites set the hint with ToolTip.text
  HoverHandler { id: hover }

  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hover.hovered && ToolTip.text.length !== 0

  Image {
    anchors.fill: parent
    source: ZeStyle.icon("drag-handle")
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
