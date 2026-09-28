import QtQuick.Controls.FluentWinUI3

SpinBox {
  editable: true

  // call sites set the hint with ToolTip.text
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hovered && ToolTip.text.length !== 0
}
