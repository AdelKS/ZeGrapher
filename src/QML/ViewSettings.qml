// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((availableWidth - mainLayout.implicitWidth)/2, 0)

    ColumnLayout {
      id: mainLayout
      anchors.centerIn: parent

      GroupBox {
        title: qsTr("Global settings")

        RowLayout {
          anchors.fill: parent
          spacing: 10

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Scaling')
          }

          ZeDoubleSpinBox {
            from: 0.1
            value: 1.0
            step: 0.1
            to: 5.0

            onValueModified: {
              Information.graphSizeSettings.scalingFactor = value;
              console.debug("global scale changed to: ", value);
            }
          }
        }
      }
    }
  }
}
