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

  contentItem: Image {
    source: Application.styleHints.colorScheme === Qt.Light ? root.lightThemeIcon : root.darkThemeIcon
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
