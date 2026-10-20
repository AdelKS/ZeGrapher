/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// The graph settings, in the panel's Graph tab

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property ZeGraphSettings graphSettings
  required property size globalMenuSize

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
    contentHeight: mainLayout.implicitHeight + root.globalMenuSize.height + 10

    ColumnLayout {
      id: mainLayout
      width: parent.width

      GroupBox {
        Layout.fillWidth: true
        title: qsTr("Size")

        GridLayout {
          columnSpacing: 10
          columns: 2

          Behavior on implicitWidth {
            NumberAnimation { duration: 500 }
          }

          ZeLabel {
            id: scalingLabel
            Layout.alignment: Qt.AlignRight
            //: how big everything is drawn, not the zoom of the view
            text: qsTr('Scaling')
            tooltipText: qsTr("Draw everything on the graph bigger or smaller.")
          }
          ZeDoubleSpinBox {
            Layout.fillWidth: true

            ToolTip.text: scalingLabel.tooltipText

            from: Limits.minScaling
            step: 0.1
            to: Limits.maxScaling

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
            model: [
              qsTr("Fill window"),
              //: give the graph a custom size, instead of making it fill the window
              qsTr("Custom"),
            ]

            currentIndex: root.graphSettings.size.sheetFillsWindow ? 0 : 1

            onActivated: (index) => {
              root.graphSettings.size.sheetFillsWindow = index === 0;

              if (index === 0) {
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

                id: sheetHeightLabel

                text: qsTr("Height")
              }
              ZeDoubleSpinBox {
                id: sheetHeight
                //: shorthand for centimeters, the size of the graph
                suffix: qsTr(" cm")
                from: Limits.minSheetCm
                to: Limits.maxSheetCm
                step: 1

                value: root.graphSettings.size.cmSheetSize.height
                onValueModified: (value) => root.graphSettings.size.cmSheetSize.height = value
              }

              ZeLabel {
                Layout.alignment: Qt.AlignRight

                id: sheetWidthLabel
                text: qsTr("Width")
              }
              ZeDoubleSpinBox {

                id: sheetWidth
                //: shorthand for centimeters, the size of the graph
                suffix: qsTr(" cm")
                from: Limits.minSheetCm
                to: Limits.maxSheetCm
                step: 1

                value: root.graphSettings.size.cmSheetSize.width
                onValueModified: (value) => root.graphSettings.size.cmSheetSize.width = value
              }

              states: [
                State {
                  name: "hidden";
                  when: root.graphSettings.size.sheetFillsWindow
                  PropertyChanges {
                    customSizeGroupBox.Layout.maximumHeight: 0
                    customSizeGroupBox.visible: false
                    emptyItem.visible: false
                  }
                },
                State {
                  name: "shown";
                  when: !root.graphSettings.size.sheetFillsWindow
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

        }
      }

      GroupBox {
        Layout.fillWidth: true
        title: qsTr("Drawing")

        GridLayout {
          columnSpacing: 10
          columns: 2

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

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Background")
          }
          ColorButton {
            id: backgroundColor
            Layout.alignment: Qt.AlignLeft

            selectedColor: root.graphSettings.backgroundColor

            onSelectedColorModified: root.graphSettings.backgroundColor = selectedColor;
          }

          ZeLabel {
            id: minPointsLabel
            Layout.alignment: Qt.AlignRight
            text: qsTr("Min points")
            tooltipText: qsTr("Every continuous curve is made of at least %1 points.").arg(Math.pow(2, minPointsLg2.value))
          }
          ZeSpinBox {
            id: minPointsLg2
            Layout.alignment: Qt.AlignLeft

            ToolTip.text: minPointsLabel.tooltipText

            from: Limits.lowestPointsLg2
            stepSize: 1
            value: root.graphSettings.minPointsLg2
            to: maxPointsLg2.value

            onValueModified: root.graphSettings.minPointsLg2 = value
          }

          ZeLabel {
            id: maxPointsLabel
            Layout.alignment: Qt.AlignRight
            text: qsTr("Max points")

            tooltipText: qsTr("Every continuous curve is made of at most %1 points.").arg(Math.pow(2, maxPointsLg2.value))
          }
          ZeSpinBox {
            id: maxPointsLg2
            Layout.alignment: Qt.AlignLeft

            ToolTip.text: maxPointsLabel.tooltipText

            from: minPointsLg2.value
            value: root.graphSettings.maxPointsLg2
            stepSize: 1
            to: Limits.highestPointsLg2

            onValueModified: root.graphSettings.maxPointsLg2 = value
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

            from: Limits.minAxisLineWidth
            step: 0.5
            to: Limits.maxAxisLineWidth

            value: root.graphSettings.axes.lineWidth
            onValueModified: (value) => root.graphSettings.axes.lineWidth = value
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Color")
          }
          ColorButton {
            id: axesColor
            Layout.alignment: Qt.AlignLeft

            selectedColor: root.graphSettings.axes.color

            onSelectedColorModified: root.graphSettings.axes.color = selectedColor;
          }

          FontDialog {
            id: fontDialog
            currentFont: root.graphSettings.axes.titleFont

            onAccepted: {
              root.graphSettings.axes.titleFont = selectedFont;
            }
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('X title')
          }
          LineEdit {
            Layout.fillWidth: true
            text: root.graphSettings.axes.x.title
            onTextEdited: root.graphSettings.axes.x.title = text
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Y title')
          }
          LineEdit {
            Layout.fillWidth: true
            text: root.graphSettings.axes.y.title
            onTextEdited: root.graphSettings.axes.y.title = text
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr('Title font')
          }
          Button {
            Layout.alignment: Qt.AlignLeft
            Layout.fillWidth: true

            text: fontDialog.currentFont.family
            onClicked: fontDialog.open()
          }
        }
      }
    }
  }
}
