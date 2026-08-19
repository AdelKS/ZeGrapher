import QtQuick
import QtQuick.Controls.FluentWinUI3

RoundButton {
  id: root
  radius: 5

  /// @brief an icon from icons/, without its '-dark' or '-light' suffix
  required property string iconName

  display: Button.IconOnly
  leftPadding: 0
  rightPadding: 0
  topPadding: 0
  bottomPadding: 0

  // an icon has no text, so call sites set the hint with ToolTip.text
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hovered && ToolTip.text.length !== 0

  contentItem: Image {
    source: ZeStyle.icon(root.iconName)
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
