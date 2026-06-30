// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property ZeGraphSettings graphSettings
  required property size globalMenuSize

  implicitWidth: scrollView.implicitWidth

  enum Selection { X_Y, X, Y }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)
    implicitWidth: mainLayout.implicitWidth + ScrollBar.vertical.width + anchors.margins * 2

    contentHeight: mainLayout.implicitHeight + root.globalMenuSize.height + 10

    ColumnLayout {
      id: mainLayout
      width: parent.width

      GroupBox {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        spacing: 0

        title: qsTr("Coordinates")

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 3

          Behavior on implicitWidth {
            NumberAnimation { duration: 500 }
          }

          Item {}
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('X')
          }
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('Y')
          }


          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Spacing')
          }
          ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 0
            value: root.graphSettings.axes.x.tickSpacing
            stepSize: 1
            to: 5

            onValueModified: root.graphSettings.axes.x.tickSpacing = value
          }
         ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 0
            value: root.graphSettings.axes.y.tickSpacing
            stepSize: 1
            to: 5

            onValueModified: root.graphSettings.axes.y.tickSpacing = value
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Max digits')
          }
          ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 2
            value: root.graphSettings.axes.x.linear.maxDigitsNum
            stepSize: 1
            to: 10

            onValueModified: root.graphSettings.axes.x.linear.maxDigitsNum = value
          }
         ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 2
            value: root.graphSettings.axes.y.linear.maxDigitsNum
            stepSize: 1
            to: 10

            onValueModified: root.graphSettings.axes.y.linear.maxDigitsNum = value
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Multiplier')
          }
          Expr {
            id: xMultiplierBackend
            implicitName: "xMultiplier"
            expression: root.graphSettings.axes.x.linear.constantMultiplierStr
            onValueChanged: {
              console.log("new x multiplier: ", value);
              root.graphSettings.axes.x.linear.constantMultiplier = value;
            }
            onExpressionChanged: root.graphSettings.axes.x.linear.constantMultiplierStr = expression;
          }
          ValueEdit {
            Layout.fillWidth: true
            backend: xMultiplierBackend
          }
          Expr {
            id: yMultiplierBackend
            implicitName: "yMultiplier"
            expression: root.graphSettings.axes.y.linear.constantMultiplierStr
            onValueChanged: root.graphSettings.axes.y.linear.constantMultiplier = value;
            onExpressionChanged: root.graphSettings.axes.y.linear.constantMultiplierStr = expression;
          }
          ValueEdit {
            Layout.fillWidth: true
            backend: yMultiplierBackend
          }
        }
      }


      GroupBox {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        spacing: 0

        title: qsTr("Grid")

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 3

          Behavior on implicitWidth {
            NumberAnimation { duration: 500 }
          }

          Item {}
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('X')
          }
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('Y')
          }


          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Enabled')
          }
          ZeCheckBox {
            Layout.alignment: Qt.AlignCenter
            display: AbstractButton.IconOnly
            checked: root.graphSettings.grid.x.show

            onCheckedChanged: {
              root.graphSettings.grid.x.show = checked;
            }
          }
          ZeCheckBox {
            Layout.alignment: Qt.AlignCenter
            display: AbstractButton.IconOnly
            checked: root.graphSettings.grid.y.show

            onCheckedChanged: {
              root.graphSettings.grid.y.show = checked;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Color')
          }
          ColorButton {
            Layout.alignment: Qt.AlignCenter
            selectedColor: root.graphSettings.grid.x.color

            onSelectedColorChanged: {
              root.graphSettings.grid.x.color = selectedColor;
            }
          }
          ColorButton {
            Layout.alignment: Qt.AlignCenter
            selectedColor: root.graphSettings.grid.y.color

            onSelectedColorChanged: {
              root.graphSettings.grid.y.color = selectedColor;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Line width')
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95
            decimals: 1

            from: 0.1
            step: 0.1
            to: 2.0

            value: root.graphSettings.grid.x.lineWidth
            onValueModified: (value) => root.graphSettings.grid.x.lineWidth = value
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95

            from: 0.1
            step: 0.1
            to: 2.0
            decimals: 1

            value: root.graphSettings.grid.y.lineWidth
            onValueModified: (value) => root.graphSettings.grid.y.lineWidth = value
          }

        }
      }

      GroupBox {
        Layout.columnSpan: 2
        Layout.fillWidth: true
        spacing: 0

        title: qsTr("Sub-grid")

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 3

          Behavior on implicitWidth {
            NumberAnimation { duration: 500 }
          }

          Item {}
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('X')
          }
          ZeLabel {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr('Y')
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Enabled')
          }
          ZeCheckBox {
            Layout.alignment: Qt.AlignCenter
            display: AbstractButton.IconOnly
            checked: root.graphSettings.subgrid.x.show

            onCheckedChanged: {
              root.graphSettings.subgrid.x.show = checked;
            }
          }
          ZeCheckBox {
            Layout.alignment: Qt.AlignCenter
            display: AbstractButton.IconOnly
            checked: root.graphSettings.subgrid.y.show

            onCheckedChanged: {
              root.graphSettings.subgrid.y.show = checked;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Color')
          }
          ColorButton {
            Layout.alignment: Qt.AlignCenter
            selectedColor: root.graphSettings.subgrid.x.color

            onSelectedColorChanged: {
              root.graphSettings.subgrid.x.color = selectedColor;
            }
          }
          ColorButton {
            Layout.alignment: Qt.AlignCenter
            selectedColor: root.graphSettings.subgrid.y.color

            onSelectedColorChanged: {
              root.graphSettings.subgrid.y.color = selectedColor;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Subdivisions')
          }
          ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 1
            value: root.graphSettings.subgrid.x.subdivs
            stepSize: 1
            to: 20

            onValueModified: root.graphSettings.subgrid.x.subdivs = value
          }
          ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 1
            value: root.graphSettings.subgrid.y.subdivs
            stepSize: 1
            to: 20

            onValueModified: root.graphSettings.subgrid.y.subdivs = value
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Line width')
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95
            decimals: 1

            from: 0.1
            step: 0.1
            to: 2.0

            value: root.graphSettings.subgrid.x.lineWidth
            onValueModified: (value) => root.graphSettings.subgrid.x.lineWidth = value
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95

            from: 0.1
            step: 0.1
            to: 2.0
            decimals: 1

            value: root.graphSettings.subgrid.y.lineWidth
            onValueModified: (value) => root.graphSettings.subgrid.y.lineWidth = value
          }

        }
      }
    }
  }
}
