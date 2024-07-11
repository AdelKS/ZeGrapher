import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import zegrapher.information 1.0

Rectangle {

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window
  implicitWidth: colLayout.implicitWidth

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    RangeAdjust {
      Layout.fillWidth: true
    }

    MathObjectInput {
      Layout.fillWidth: true
    }
  }
}
