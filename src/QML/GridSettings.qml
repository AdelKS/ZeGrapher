// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  property bool pauseSync: false

  required property ZeGraphSettings graphSettings

  implicitWidth: scrollView.implicitWidth

  enum Selection { X_Y, X, Y }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)
    implicitWidth: mainLayout.implicitWidth + ScrollBar.vertical.width + anchors.margins * 2

    ColumnLayout {
      id: mainLayout
      anchors.fill: parent

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
            value: 0
            stepSize: 1
            to: 5

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.axes.x.tickSpacing = value;
              console.info("x axis tick spacing: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
          }
         ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 0
            value: 0
            stepSize: 1
            to: 5

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.axes.y.tickSpacing = value;
              console.info("y axis tick spacing: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
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
            value: 4
            stepSize: 1
            to: 10

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.axes.x.linear.maxDigitsNum = value;
              console.info("x axis max digit num: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
          }
         ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 2
            value: 4
            stepSize: 1
            to: 10

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.axes.y.linear.maxDigitsNum = value;
              console.info("y axis max digit num: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Multiplier')
          }
          ValueEdit {
            Layout.fillWidth: true

            backend: MathWorld.addAltExprObject()

            Component.onCompleted: {
              backend.setImplicitName("xMultiplier");
              root.graphSettings.axes.x.linear.constantMultiplier = backend;
            }
            Component.onDestruction: MathWorld.removeAltExprObject(backend)
          }
         ValueEdit {
            Layout.fillWidth: true

           backend: MathWorld.addAltExprObject()

            Component.onCompleted: {
              backend.setImplicitName("yMultiplier");
              root.graphSettings.axes.y.linear.constantMultiplier = backend;
            }
            Component.onDestruction: MathWorld.removeAltExprObject(backend)
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

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.grid.x.lineWidth = value;
              console.info("x grid line width: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
            Component.onCompleted: {
              setValue(root.graphSettings.grid.x.lineWidth);
            }
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95

            from: 0.1
            step: 0.1
            to: 2.0
            decimals: 1

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.grid.y.lineWidth = value;
              console.info("y grid line width: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
            Component.onCompleted: {
              setValue(root.graphSettings.grid.y.lineWidth);
            }
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

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.subgrid.x.subdivs = value;
              console.info("x subgrid subdivs: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
          }
          ZeSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            from: 1
            value: root.graphSettings.subgrid.y.subdivs
            stepSize: 1
            to: 20

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.subgrid.y.subdivs = value;
              console.info("y subgrid subdivs: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
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

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.subgrid.x.lineWidth = value;
              console.info("x grid line width: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
            Component.onCompleted: {
              setValue(root.graphSettings.subgrid.x.lineWidth);
            }
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true
            Layout.minimumWidth: 95
            implicitWidth: 95

            from: 0.1
            step: 0.1
            to: 2.0
            decimals: 1

            function updateBackend() {
              root.pauseSync = true;
              root.graphSettings.subgrid.y.lineWidth = value;
              console.info("y grid line width: ", value);
              root.pauseSync = false;
            }
            onValueModified: updateBackend()
            Component.onCompleted: {
              setValue(root.graphSettings.subgrid.y.lineWidth);
            }
          }

        }
      }
    }
  }
}
