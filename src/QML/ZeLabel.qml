import QtQuick.Controls.FluentWinUI3
import QtQuick

Label {

  property string tooltipText: ""

  font: Information.appSettings.font

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.NoButton
  }

  ToolTip.text: tooltipText
  ToolTip.visible: mouseArea.containsMouse && tooltipText.length !== 0
}
