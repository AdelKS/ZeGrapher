// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  readonly property double cm_increment: 1.0;
  readonly property double px_increment: 50;
  readonly property double max_px_size: 10000
  readonly property double max_cm_size: 1000

  property bool pauseSync: false

  required property ZeGraphSettings graphSettings

  enum SizeType { Fill, Custom }

  FontDialog {
    id: graphFontDialog
    currentFont: root.graphSettings.font

    onAccepted: {
      root.graphSettings.font = selectedFont;
    }
  }

  function syncWithBackend() {
    console.log("Syncing size settings with backend")
    if (root.graphSettings.size.sizeUnit === SizeUnit.PIXEL) {
      sheetHeight.suffix = qsTr(" px");
      sheetHeight.to = root.max_px_size;
      sheetHeight.setValue(root.graphSettings.size.pxSheetSize.height);
      sheetHeight.step = root.px_increment;

      sheetWidth.suffix = qsTr(" px");
      sheetWidth.to = root.max_px_size;
      sheetWidth.setValue(root.graphSettings.size.pxSheetSize.width);
      sheetWidth.step = root.px_increment;
    } else {
      sheetHeight.suffix = qsTr(" cm");
      sheetHeight.to = root.max_cm_size;
      sheetHeight.setValue(root.graphSettings.size.cmSheetSize.height);
      sheetHeight.step = root.cm_increment;

      sheetWidth.suffix = qsTr(" cm");
      sheetWidth.to = root.max_cm_size;
      sheetWidth.setValue(root.graphSettings.size.cmSheetSize.width);
      sheetWidth.step = root.cm_increment;
    }
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)

    ColumnLayout {
      id: mainLayout
      anchors.fill: parent

      GroupBox {
        Layout.fillWidth: true
        title: qsTr("Global settings")

        GridLayout {
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
              root.graphSettings.size.scalingFactor = value;
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
                value: GraphSettings.SizeType.Fill
              }
              ListElement {
                text: qsTr("Custom")
                value: GraphSettings.SizeType.Custom
              }
            }

            onCurrentValueChanged: {
              root.pauseSync = true;
              root.graphSettings.size.sheetFillsWindow = (currentValue === GraphSettings.SizeType.Fill)

              if (currentValue === GraphSettings.SizeType.Fill) {
                root.graphSettings.zoom.zoom = 1.0;
                root.graphSettings.zoom.zoomingType = ZoomingType.FITSHEET;
              }
              root.pauseSync = false;
            }
          }

          Item {
            id: emptyItem
          }
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
                target: root.graphSettings

                function onSizeChanged() {
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
                  root.graphSettings.size.sizeUnit = currentValue;
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

                onValueModified: {
                  root.pauseSync = true;
                  console.log("Updating sheet size");
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    root.graphSettings.size.pxSheetSize.height = value;
                    root.graphSettings.size.cmSheetSize.height = value / Information.pixelDensity;
                  } else {
                    root.graphSettings.size.pxSheetSize.height = value * Information.pixelDensity;
                    root.graphSettings.size.cmSheetSize.height = value;
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
                  function onCurrentValueChanged() {
                    sheetWidth.suffix = unitComboBox.currentValue === SizeUnit.PIXEL ? qsTr(" px") : qsTr(" cm")
                  }
                }

                onValueModified: {
                  root.pauseSync = true;
                  if (unitComboBox.currentValue === SizeUnit.PIXEL) {
                    root.graphSettings.size.pxSheetSize.width = value;
                    root.graphSettings.size.cmSheetSize.width = value / Information.pixelDensity;
                  } else {
                    root.graphSettings.size.pxSheetSize.width = value * Information.pixelDensity;
                    root.graphSettings.size.cmSheetSize.width = value;
                  }
                  root.pauseSync = false;
                }
              }

              states: [
                State {
                  name: "hidden";
                  when: sheetSizeSettings.currentValue === GraphSettings.SizeType.Fill
                  PropertyChanges {
                    customSizeGroupBox.maxHeight: 0
                    customSizeGroupBox.visible: false
                    emptyItem.visible: false
                  }
                },
                State {
                  name: "shown";
                  when: sheetSizeSettings.currentValue !== GraphSettings.SizeType.Fill
                  PropertyChanges {
                    customSizeGroupBox.maxHeight: customSizeGroupBox.implicitHeight
                    customSizeGroupBox.visible: true
                    emptyItem.visible: true
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

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Font')
          }
          Button {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft
            text: graphFontDialog.currentFont.family
            onClicked: graphFontDialog.open()
          }

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Background")
          }
          ColorButton {
            id: backgroundColor
            Layout.alignment: Qt.AlignLeft

            selectedColor: { selectedColor = root.graphSettings.backgroundColor; }

            onSelectedColorChanged: root.graphSettings.backgroundColor = selectedColor;

            Connections {
              target: root.graphSettings

              function onBackgroundColorChanged() {
                if (backgroundColor.selectedColor !== root.graphSettings.backgroundColor) {
                  backgroundColor.selectedColor = root.graphSettings.backgroundColor
                }
              }
            }
          }
        }
      }

      GroupBox {
        Layout.fillWidth: true
        title: qsTr("Axes")

        GridLayout {
          columnSpacing: 10
          columns: 2

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Line width')
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true

            from: 0.1
            step: 0.5
            to: 5.0

            onValueModified: {
              root.pauseSync = true;
              root.graphSettings.axes.lineWidth = value;
              console.debug("Axes line width changed: ", value);
              root.pauseSync = false;
            }

            Component.onCompleted: {
              setValue(graphSettings.axes.lineWidth);
            }
          }

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Color")
          }
          ColorButton {
            id: axesColor
            Layout.alignment: Qt.AlignLeft

            selectedColor: { selectedColor = root.graphSettings.axes.color; }

            onSelectedColorChanged: root.graphSettings.axes.color = selectedColor;

            Connections {
              target: root.graphSettings

              function onAxesChanged() {
                if (axesColor.selectedColor !== root.graphSettings.axes.color) {
                  axesColor.selectedColor = root.graphSettings.axes.color
                }
              }
            }
          }
        }
      }
    }
  }
}
