import QtQuick
import QtQuick.Controls.FluentWinUI3

Button {
  id: root

  required property url lightThemeIcon
  required property url darkThemeIcon

  display: Button.IconOnly
  leftPadding: 0
  rightPadding: 0
  topPadding: 0
  bottomPadding: 0

  // an icon carries no text, so call sites give the hint with ToolTip.text
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hovered && ToolTip.text.length !== 0

  contentItem: Image {
    source: Application.styleHints.colorScheme === Qt.Light ? root.lightThemeIcon : root.darkThemeIcon
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
