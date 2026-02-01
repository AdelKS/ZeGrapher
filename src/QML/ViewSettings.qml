// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  readonly property double cm_per_inch: 2.54
  readonly property double cm_increment: 1;
  readonly property double px_increment: 50;
  readonly property double max_px_size: 10000
  readonly property double max_cm_size: 1000

  enum SizeType { Fill, Custom }

  function syncWithBackend() {
    if (Information.graphSizeSettings.sizeUnit === SizeUnit.PIXEL) {
      sheetHeight.suffix = qsTr(" px");
      sheetHeight.to = root.max_px_size;
      sheetHeight.value = Information.graphSizeSettings.pxSheetSize.height;
      sheetHeight.step = root.px_increment;

      sheetWidth.suffix = qsTr(" px");
      sheetWidth.to = root.max_px_size;
      sheetWidth.value = Information.graphSizeSettings.pxSheetSize.width;
      sheetWidth.step = root.px_increment;

      graphHeight.suffix = qsTr(" px");
      graphHeight.value = Information.graphSizeSettings.pxFigureSize.height;
      graphHeight.step = root.px_increment;

      graphWidth.suffix = qsTr(" px");
      graphWidth.value = Information.graphSizeSettings.pxFigureSize.width;
      graphWidth.step = root.px_increment;
    } else {
      sheetHeight.suffix = qsTr(" cm");
      sheetHeight.to = root.max_cm_size;
      sheetHeight.value = Information.graphSizeSettings.cmSheetSize.height;
      sheetHeight.step = root.cm_increment;

      sheetWidth.suffix = qsTr(" cm");
      sheetWidth.to = root.max_cm_size;
      sheetWidth.value = Information.graphSizeSettings.cmSheetSize.width;
      sheetWidth.step = root.cm_increment;

      graphHeight.suffix = qsTr(" cm");
      graphHeight.value = Information.graphSizeSettings.cmFigureSize.height;
      graphHeight.step = root.cm_increment;

      graphWidth.suffix = qsTr(" cm");
      graphWidth.value = Information.graphSizeSettings.cmFigureSize.width;
      graphWidth.step = root.cm_increment;
    }
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)

    ColumnLayout {
      id: mainLayout
      // anchors.centerIn: parent

      GroupBox {
        title: qsTr("Global settings")

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 2

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Scaling')
          }
          ZeDoubleSpinBox {
            from: 0.1
            value: Information.graphSizeSettings.scalingFactor
            step: 0.1
            to: 5.0
            decimals: 2

            onValueModified: {
              Information.graphSizeSettings.defaultScaling = false;
              Information.graphSizeSettings.scalingFactor = value;
              console.debug("global scale changed to: ", value);
            }
          }
        }
      }

      GroupBox {
        title: qsTr("Sizes")

        GridLayout {
          anchors.fill: parent
          columnSpacing: 10
          columns: 2


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
              Information.graphSizeSettings.sizeUnit = currentValue;
              root.syncWithBackend();
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

          GroupBox {
            Layout.columnSpan: 2
            Layout.fillWidth: true

            title: qsTr("Sheet")

            GridLayout {
              anchors.fill: parent
              columnSpacing: 10
              columns: 2

              ComboBox {
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
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
                  Information.graphSizeSettings.sheetFillsWindow = (currentValue === ViewSettings.SizeType.Fill)
                }
              }

              Connections {
                target: Information

                function onGraphSizeSettingsChanged() {
                  root.syncWithBackend()
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
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    Information.graphSizeSettings.pxSheetSize.height = value;
                    Information.graphSizeSettings.cmSheetSize.height = value / Information.screenDpi * cm_per_inch;
                  } else {
                    Information.graphSizeSettings.pxSheetSize.height = value * Information.screenDpi / cm_per_inch;
                    Information.graphSizeSettings.cmSheetSize.height = value;
                  }
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
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    Information.graphSizeSettings.pxSheetSize.width = value;
                    Information.graphSizeSettings.cmSheetSize.width = value / Information.screenDpi * cm_per_inch;
                  } else {
                    Information.graphSizeSettings.pxSheetSize.width = value * Information.screenDpi / cm_per_inch;
                    Information.graphSizeSettings.cmSheetSize.width = value;
                  }
                }
              }

              states: [
                State {
                  name: "hidden";
                  when: sheetSizeSettings.currentValue === ViewSettings.SizeType.Fill
                  PropertyChanges {
                    sheetWidthLabel.maxHeight: 0
                    sheetWidthLabel.visible: false

                    sheetWidth.maxHeight: 0
                    sheetWidth.visible: false

                    sheetHeightLabel.maxHeight: 0
                    sheetHeightLabel.visible: false

                    sheetHeight.maxHeight: 0
                    sheetHeight.visible: false
                  }
                },
                State {
                  name: "shown";
                  when: sheetSizeSettings.currentValue !== ViewSettings.SizeType.Fill
                  PropertyChanges {
                    sheetWidthLabel.maxHeight: sheetWidthLabel.implicitHeight
                    sheetWidthLabel.visible: true

                    sheetWidth.maxHeight: sheetWidth.implicitHeight
                    sheetWidth.visible: true

                    sheetHeightLabel.maxHeight: sheetHeightLabel.implicitHeight
                    sheetHeightLabel.visible: true

                    sheetHeight.maxHeight: sheetHeight.implicitHeight
                    sheetHeight.visible: true
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


          GroupBox {
            Layout.columnSpan: 2
            Layout.fillWidth: true

            title: qsTr("Graph")

            GridLayout {
              anchors.fill: parent
              columnSpacing: 10
              columns: 2

              ComboBox {
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.fillWidth: true

                id: graphSizeSettings

                implicitContentWidthPolicy: ComboBox.WidestText
                background.implicitWidth: implicitContentWidth
                textRole: "text"
                valueRole: "value"

                model: ListModel {
                  ListElement {
                    text: qsTr("Fill Sheet")
                    value: ViewSettings.SizeType.Fill
                  }
                  ListElement {
                    text: qsTr("Custom")
                    value: ViewSettings.SizeType.Custom
                  }
                }

                onCurrentValueChanged: {
                  Information.graphSizeSettings.figureFillsSheet = (currentValue === ViewSettings.SizeType.Fill)
                }
              }

              ZeLabel {
                Layout.alignment: Qt.AlignRight
                Layout.maximumHeight: maxHeight

                property int maxHeight: implicitHeight

                id: graphHeightLabel

                text: qsTr("Height")
              }
              ZeDoubleSpinBox {
                Layout.maximumHeight: maxHeight
                property int maxHeight: implicitHeight

                id: graphHeight
                suffix: unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")

                Connections {
                  target: unitComboBox
                  function onValueChanged() {
                    suffix = unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")
                  }
                }

                onValueModified: {
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    Information.graphSizeSettings.pxFigureSize.height = value;
                    Information.graphSizeSettings.cmFigureSize.height = value / Information.screenDpi * cm_per_inch;
                  } else {
                    Information.graphSizeSettings.pxFigureSize.height = value * Information.screenDpi / cm_per_inch;
                    Information.graphSizeSettings.cmFigureSize.height = value;
                  }
                }
              }

              ZeLabel {
                Layout.alignment: Qt.AlignRight
                Layout.maximumHeight: maxHeight
                property int maxHeight: implicitHeight

                id: graphWidthLabel
                text: qsTr("Width")
              }
              ZeDoubleSpinBox {
                Layout.maximumHeight: maxHeight
                property int maxHeight: implicitHeight

                id: graphWidth
                suffix: unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")

                Connections {
                  target: unitComboBox
                  function onValueChanged() {
                    suffix = unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")
                  }
                }

                onValueModified: {
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    Information.graphSizeSettings.pxFigureSize.width = value;
                    Information.graphSizeSettings.cmFigureSize.width = value / Information.screenDpi * cm_per_inch;
                  } else {
                    Information.graphSizeSettings.pxFigureSize.width = value * Information.screenDpi / cm_per_inch;
                    Information.graphSizeSettings.cmFigureSize.width = value;
                  }
                }
              }

              states: [
                State {
                  name: "hidden";
                  when: graphSizeSettings.currentValue === ViewSettings.SizeType.Fill
                  PropertyChanges {
                    graphWidthLabel.maxHeight: 0
                    graphWidthLabel.visible: false

                    graphWidth.maxHeight: 0
                    graphWidth.visible: false

                    graphHeightLabel.maxHeight: 0
                    graphHeightLabel.visible: false

                    graphHeight.maxHeight: 0
                    graphHeight.visible: false
                  }
                },
                State {
                  name: "shown";
                  when: graphSizeSettings.currentValue !== ViewSettings.SizeType.Fill
                  PropertyChanges {
                    graphWidthLabel.maxHeight: graphWidthLabel.implicitHeight
                    graphWidthLabel.visible: true

                    graphWidth.maxHeight: graphWidth.implicitHeight
                    graphWidth.visible: true

                    graphHeightLabel.maxHeight: graphHeightLabel.implicitHeight
                    graphHeightLabel.visible: true

                    graphHeight.maxHeight: graphHeight.implicitHeight
                    graphHeight.visible: true
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
  }
}
