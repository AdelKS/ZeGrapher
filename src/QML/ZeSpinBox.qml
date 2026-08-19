import QtQuick.Controls.FluentWinUI3

SpinBox {
  font: Information.appSettings.font
  editable: true

  // call sites give the hint with ToolTip.text
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hovered && ToolTip.text.length !== 0
}
