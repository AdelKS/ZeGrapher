import QtQuick
import QtQuick.Controls.FluentWinUI3

TabButton {
  id: root
  contentItem: Text {
    text: root.text
    font: Information.appSettings.font
    color: palette.text
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }
}
