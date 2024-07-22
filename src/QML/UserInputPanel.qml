import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import zegrapher.information 1.0

Item {

  implicitWidth: colLayout.implicitWidth

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    RangeAdjust {
      Layout.fillWidth: true
      Layout.bottomMargin: 20
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
      Layout.bottomMargin: 20
    }

    MathObjectInput {
      Layout.fillWidth: true
    }
  }
}
