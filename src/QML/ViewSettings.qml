// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  readonly property double cm_increment: 1.0;
  readonly property double px_increment: 50;
  readonly property double max_px_size: 10000
  readonly property double max_cm_size: 1000

  property bool pauseSync: false

  enum SizeType { Fill, Custom }

  function syncWithBackend() {
    if (Information.graphSizeSettings.sizeUnit === SizeUnit.PIXEL) {
      sheetHeight.suffix = qsTr(" px");
      sheetHeight.to = root.max_px_size;
      sheetHeight.setValue(Information.graphSizeSettings.pxSheetSize.height);
      sheetHeight.step = root.px_increment;

      sheetWidth.suffix = qsTr(" px");
      sheetWidth.to = root.max_px_size;
      sheetWidth.setValue(Information.graphSizeSettings.pxSheetSize.width);
      sheetWidth.step = root.px_increment;
    } else {
      sheetHeight.suffix = qsTr(" cm");
      sheetHeight.to = root.max_cm_size;
      sheetHeight.setValue(Information.graphSizeSettings.cmSheetSize.height);
      sheetHeight.step = root.cm_increment;

      sheetWidth.suffix = qsTr(" cm");
      sheetWidth.to = root.max_cm_size;
      sheetWidth.setValue(Information.graphSizeSettings.cmSheetSize.width);
      sheetWidth.step = root.cm_increment;
    }
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)

    GridLayout {
      id: mainLayout
      columnSpacing: 10
      columns: 2

      Behavior on implicitWidth {
        NumberAnimation { duration: 500 }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Scaling')
      }
      ZeDoubleSpinBox {
        Layout.fillWidth: true

        from: 0.1
        step: 0.1
        to: 5.0

        onValueModified: {
          root.pauseSync = true;
          Information.graphSizeSettings.scalingFactor = value;
          console.debug("global scale changed to: ", value);
          root.pauseSync = false;
        }

        Component.onCompleted: {
          setValue(1.0);
        }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Graph size')
      }
      ComboBox {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true

        id: sheetSizeSettings

        implicitContentWidthPolicy: ComboBox.WidestText
        background.implicitWidth: implicitContentWidth
        textRole: "text"
        valueRole: "value"

        model: ListModel {
          ListElement {
            text: qsTr("Fill Window")
            value: ViewSettings.SizeType.Fill
          }
          ListElement {
            text: qsTr("Custom")
            value: ViewSettings.SizeType.Custom
          }
        }

        onCurrentValueChanged: {
          root.pauseSync = true;
          Information.graphSizeSettings.sheetFillsWindow = (currentValue === ViewSettings.SizeType.Fill)

          if (currentValue === ViewSettings.SizeType.Fill) {
            Information.graphZoomSettings.zoom = 1.0;
            Information.graphZoomSettings.zoomingType = ZoomingType.FITSHEET;
          }
          root.pauseSync = false;
        }
      }

      Item {}
      Frame {
        clip: true
        Layout.topMargin: 0
        Layout.fillWidth: true

        Layout.maximumHeight: maxHeight

        property int maxHeight: implicitHeight

        id: customSizeGroupBox

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 2

          Connections {
            target: Information

            function onGraphSizeSettingsChanged() {
              root.syncWithBackend()
            }
            enabled: !root.pauseSync
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Unit')
          }
          ComboBox {
            Layout.fillWidth: true

            id: unitComboBox

            implicitContentWidthPolicy: ComboBox.WidestText
            background.implicitWidth: implicitContentWidth
            textRole: "text"
            valueRole: "value"

            onCurrentValueChanged: {
              root.pauseSync = true;
              Information.graphSizeSettings.sizeUnit = currentValue;
              root.syncWithBackend();
              root.pauseSync = false;
            }

            model: ListModel {
              ListElement {
                text: qsTr("Pixels")
                value: SizeUnit.PIXEL
              }
              ListElement {
                text: qsTr("Centimeters")
                value: SizeUnit.CENTIMETER
              }
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            Layout.maximumHeight: maxHeight
            property int maxHeight: implicitHeight

            id: sheetHeightLabel

            text: qsTr("Height")
          }
          ZeDoubleSpinBox {
            Layout.maximumHeight: maxHeight
            property int maxHeight: implicitHeight

            id: sheetHeight

            onValueChanged: {
              graphHeight.to = value;
            }

            onValueModified: {
              root.pauseSync = true;
              console.log("Updating sheet size");
              if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                Information.graphSizeSettings.pxSheetSize.height = value;
                Information.graphSizeSettings.cmSheetSize.height = value / Information.pixelDensity;
              } else {
                Information.graphSizeSettings.pxSheetSize.height = value * Information.pixelDensity;
                Information.graphSizeSettings.cmSheetSize.height = value;
              }
              root.pauseSync = false;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            Layout.maximumHeight: maxHeight
            property int maxHeight: implicitHeight

            id: sheetWidthLabel
            text: qsTr("Width")
          }
          ZeDoubleSpinBox {
            Layout.maximumHeight: maxHeight
            property int maxHeight: implicitHeight

            id: sheetWidth
            suffix: unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")

            Connections {
              target: unitComboBox
              function onValueChanged() {
                suffix = unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")
              }
            }

            onValueChanged: {
              graphWidth.to = value;
            }

            onValueModified: {
              root.pauseSync = true;
              if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                Information.graphSizeSettings.pxSheetSize.width = value;
                Information.graphSizeSettings.cmSheetSize.width = value / Information.pixelDensity;
              } else {
                Information.graphSizeSettings.pxSheetSize.width = value * Information.pixelDensity;
                Information.graphSizeSettings.cmSheetSize.width = value;
              }
              root.pauseSync = false;
            }
          }

          states: [
            State {
              name: "hidden";
              when: sheetSizeSettings.currentValue === ViewSettings.SizeType.Fill
              PropertyChanges {
                customSizeGroupBox.maxHeight: 0
                customSizeGroupBox.visible: false
              }
            },
            State {
              name: "shown";
              when: sheetSizeSettings.currentValue !== ViewSettings.SizeType.Fill
              PropertyChanges {
                customSizeGroupBox.maxHeight: customSizeGroupBox.implicitHeight
                customSizeGroupBox.visible: true
              }
            }
          ]

          transitions: Transition {
            reversible: true
            from: "shown"
            to: "hidden"
            SequentialAnimation {
              NumberAnimation {
                easing.type: Easing.InOutQuad;
                property: "maxHeight";
                duration: 400;
              }
              PropertyAction {
                property: "visible"
              }
            }
          }
        }
      }
    }
  }
}
