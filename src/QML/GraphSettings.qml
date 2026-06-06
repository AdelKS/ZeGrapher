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

  required property ZeGraphSettings graphSettings

  enum SizeType { Fill, Custom }

  readonly property bool pixelUnits: root.graphSettings.size.sizeUnit === SizeUnit.PIXEL

  function indexFromModelValue(model: var, val: int) : int {
    for (var i = 0; i !== model.count; i++) {
      if (model.get(i).value === val)
        return i;
    }
    console.warn("Value not in model: ", val)
    return -1;
  }

  FontDialog {
    id: graphFontDialog
    currentFont: root.graphSettings.font

    onAccepted: {
      root.graphSettings.font = selectedFont;
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

            value: root.graphSettings.size.scalingFactor
            onValueModified: (value) => root.graphSettings.size.scalingFactor = value
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

            model: sheetSizeModel

            ListModel {
              id: sheetSizeModel
              ListElement {
                text: qsTr("Fill Window")
                value: GraphSettings.SizeType.Fill
              }
              ListElement {
                text: qsTr("Custom")
                value: GraphSettings.SizeType.Custom
              }
            }

            currentIndex: root.indexFromModelValue(sheetSizeModel, root.graphSettings.size.sheetFillsWindow ? GraphSettings.SizeType.Fill : GraphSettings.SizeType.Custom)

            onActivated: (index) => {
              root.graphSettings.size.sheetFillsWindow = ( sheetSizeModel.get(index).value === GraphSettings.SizeType.Fill);

              if (currentValue === GraphSettings.SizeType.Fill) {
                root.graphSettings.zoom.zoom = 1.0;
                root.graphSettings.zoom.zoomingType = ZoomingType.FITSHEET;
              }
            }
          }

          Item {
            id: emptyItem
          }
          Frame {
            clip: true
            Layout.topMargin: 0
            Layout.fillWidth: true

            id: customSizeGroupBox

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

                currentIndex: root.indexFromModelValue(unitModel, root.graphSettings.size.sizeUnit)

                onActivated: (index) => {
                  root.graphSettings.size.sizeUnit = unitModel.get(index).value;
                }

                model: unitModel

                ListModel {
                  id: unitModel
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

                id: sheetHeightLabel

                text: qsTr("Height")
              }
              ZeDoubleSpinBox {
                id: sheetHeight
                suffix: root.pixelUnits ? qsTr(" px") : qsTr(" cm")
                to: root.pixelUnits ? root.max_px_size : root.max_cm_size
                step: root.pixelUnits ? root.px_increment : root.cm_increment

                value: root.pixelUnits ? root.graphSettings.size.pxSheetSize.height : root.graphSettings.size.cmSheetSize.height

                onValueModified: (value) => {

                  console.log("Updating sheet size");
                  if (root.pixelUnits) {
                    root.graphSettings.size.pxSheetSize.height = value;
                    root.graphSettings.size.cmSheetSize.height = value / Information.pixelDensity;
                  } else {
                    root.graphSettings.size.pxSheetSize.height = value * Information.pixelDensity;
                    root.graphSettings.size.cmSheetSize.height = value;
                  }

                }
              }

              ZeLabel {
                Layout.alignment: Qt.AlignRight

                id: sheetWidthLabel
                text: qsTr("Width")
              }
              ZeDoubleSpinBox {

                id: sheetWidth
                suffix: root.pixelUnits ? qsTr(" px") : qsTr(" cm")
                to: root.pixelUnits ? root.max_px_size : root.max_cm_size
                step: root.pixelUnits ? root.px_increment : root.cm_increment

                value: root.pixelUnits ? root.graphSettings.size.pxSheetSize.width : root.graphSettings.size.cmSheetSize.width

                onValueModified: (value) => {
                  if (root.pixelUnits) {
                    root.graphSettings.size.pxSheetSize.width = value;
                    root.graphSettings.size.cmSheetSize.width = value / Information.pixelDensity;
                  } else {
                    root.graphSettings.size.pxSheetSize.width = value * Information.pixelDensity;
                    root.graphSettings.size.cmSheetSize.width = value;
                  }
                }
              }

              states: [
                State {
                  name: "hidden";
                  when: sheetSizeSettings.currentValue === GraphSettings.SizeType.Fill
                  PropertyChanges {
                    customSizeGroupBox.Layout.maximumHeight: 0
                    customSizeGroupBox.visible: false
                    emptyItem.visible: false
                  }
                },
                State {
                  name: "shown";
                  when: sheetSizeSettings.currentValue !== GraphSettings.SizeType.Fill
                  PropertyChanges {
                    customSizeGroupBox.Layout.maximumHeight: customSizeGroupBox.implicitHeight
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
                    property: "maximumHeight";
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

            selectedColor: root.graphSettings.backgroundColor

            onSelectedColorChanged: root.graphSettings.backgroundColor = selectedColor;
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Min points")
            tooltipText: qsTr("The minimum amount of points that each continuous curve should have, in 2^value.")
          }
          ZeSpinBox {
            id: minPointsLg2
            Layout.alignment: Qt.AlignLeft

            from: 2
            stepSize: 1
            value: root.graphSettings.minPointsLg2
            to: maxPointsLg2.value

            onValueModified: {
              root.graphSettings.minPointsLg2 = value;
              console.debug("Min points changed: ", value);
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Max points")
            tooltipText: qsTr("The maximum amount of points that continuous curves can have, in 2^value.")
          }
          ZeSpinBox {
            id: maxPointsLg2
            Layout.alignment: Qt.AlignLeft

            from: minPointsLg2.value
            value: root.graphSettings.maxPointsLg2
            stepSize: 1
            to: 16

            onValueModified: {
              root.graphSettings.maxPointsLg2 = value;
              console.debug("Max points changed: ", value);
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

            value: root.graphSettings.axes.lineWidth
            onValueModified: (value) => root.graphSettings.axes.lineWidth = value
          }

          Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Color")
          }
          ColorButton {
            id: axesColor
            Layout.alignment: Qt.AlignLeft

            selectedColor: root.graphSettings.axes.color

            onSelectedColorChanged: root.graphSettings.axes.color = selectedColor;
          }
        }
      }
    }
  }
}
