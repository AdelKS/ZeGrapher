import QtQuick
import QtQuick.Controls.FluentWinUI3

RoundButton {
  id: root
  radius: 5

  required property url lightThemeIcon
  required property url darkThemeIcon

  display: Button.IconOnly
  leftPadding: 0
  rightPadding: 0
  topPadding: 0
  bottomPadding: 0

  contentItem: Image {
    source: ZeStyle.dark ? root.darkThemeIcon : root.lightThemeIcon
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
