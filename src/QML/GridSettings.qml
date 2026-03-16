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
        }
      }
    }
  }
}
