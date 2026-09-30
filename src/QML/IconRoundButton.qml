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

  contentItem: Image {
    source: ZeStyle.icon(root.iconName)
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
