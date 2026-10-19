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

// The grid and axis settings, in the panel's Grid tab

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property ZeGraphSettings graphSettings
  required property size globalMenuSize

  /// @brief a box of settings: one row per setting, one column per axis. Each
  ///        row is a label, then a Repeater over the axes
  component AxesBox: GroupBox {
    default property alias rows: grid.data

    Layout.fillWidth: true
    spacing: 0

    GridLayout {
      id: grid
      anchors.fill: parent
      columnSpacing: 10
      columns: 3

      Behavior on implicitWidth {
        NumberAnimation { duration: 500 }
      }

      Item {}
      ZeLabel {
        Layout.alignment: Qt.AlignHCenter
        text: 'X'
      }
      ZeLabel {
        Layout.alignment: Qt.AlignHCenter
        text: 'Y'
      }
    }
  }

  /// @brief the line settings of the grid or sub-grid
  component LinesBox: AxesBox {
    id: box

    required property ZeGraphSettings graphSettings

    /// @brief 'grid' or 'subgrid', the property of ZeGraphSettings to edit
    required property string lines

    ZeLabel {
      Layout.alignment: Qt.AlignRight
      text: qsTr('Enabled')
    }
    Repeater {
      model: ["x", "y"]
      ZeCheckBox {
        required property string modelData
        Layout.alignment: Qt.AlignCenter
        display: AbstractButton.IconOnly
        checked: box.graphSettings[box.lines][modelData].show
        onToggled: box.graphSettings[box.lines][modelData].show = checked
      }
    }

    ZeLabel {
      Layout.alignment: Qt.AlignRight
      text: qsTr('Color')
    }
    Repeater {
      model: ["x", "y"]
      ColorButton {
        required property string modelData
        Layout.alignment: Qt.AlignCenter
        selectedColor: box.graphSettings[box.lines][modelData].color
        onSelectedColorModified: box.graphSettings[box.lines][modelData].color = selectedColor
      }
    }

    ZeLabel {
      Layout.alignment: Qt.AlignRight
      visible: box.lines === "subgrid"
      text: qsTr('Subdivisions')
    }
    Repeater {
      model: box.lines === "subgrid" ? ["x", "y"] : []
      ZeSpinBox {
        required property string modelData
        Layout.fillWidth: true
        Layout.minimumWidth: 85
        implicitWidth: 85

        from: 1
        value: box.graphSettings.subgrid[modelData].subdivs
        stepSize: 1
        to: 20

        onValueModified: box.graphSettings.subgrid[modelData].subdivs = value
      }
    }

    ZeLabel {
      Layout.alignment: Qt.AlignRight
      text: qsTr('Line width')
    }
    Repeater {
      model: ["x", "y"]
      ZeDoubleSpinBox {
        required property string modelData
        Layout.fillWidth: true
        Layout.minimumWidth: 95
        implicitWidth: 95

        from: 0.1
        step: 0.1
        to: 2.0
        decimals: 1

        value: box.graphSettings[box.lines][modelData].lineWidth
        onValueModified: (value) => box.graphSettings[box.lines][modelData].lineWidth = value
      }
    }
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)

    contentHeight: mainLayout.implicitHeight + root.globalMenuSize.height + 10

    ColumnLayout {
      id: mainLayout
      width: parent.width

      AxesBox {
        title: qsTr("Coordinates")

        ZeLabel {
          id: spacingLabel
          Layout.alignment: Qt.AlignRight
          text: qsTr('Spacing')
          tooltipText: qsTr("Spreads the ticks further apart. Zero is the tightest spacing the app draws.")
        }
        Repeater {
          model: ["x", "y"]
          ZeSpinBox {
            required property string modelData
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            ToolTip.text: spacingLabel.tooltipText

            from: 0
            value: root.graphSettings.axes[modelData].tickSpacing
            stepSize: 1
            to: 5

            onValueModified: root.graphSettings.axes[modelData].tickSpacing = value
          }
        }

        ZeLabel {
          id: maxDigitsLabel
          Layout.alignment: Qt.AlignRight
          text: qsTr('Max digits')
          tooltipText: qsTr("The number of digits a tick label can use before the app writes a power of ten.")
        }
        Repeater {
          model: ["x", "y"]
          ZeSpinBox {
            required property string modelData
            Layout.fillWidth: true
            Layout.minimumWidth: 85
            implicitWidth: 85

            ToolTip.text: maxDigitsLabel.tooltipText

            from: 2
            value: root.graphSettings.axes[modelData].linear.maxDigitsNum
            stepSize: 1
            to: 10

            onValueModified: root.graphSettings.axes[modelData].linear.maxDigitsNum = value
          }
        }

        ZeLabel {
          id: multiplierLabel
          Layout.alignment: Qt.AlignRight
          //: the ticks become multiples of this expression
          text: qsTr('Multiplier')
          tooltipText: qsTr("Ticks and their labels become multiples of this expression, for example math::pi.")
        }
        Repeater {
          model: ["x", "y"]
          ValueEdit {
            id: multiplierEdit
            required property string modelData
            Layout.fillWidth: true
            backend: root.graphSettings[modelData + "Multiplier"]
            ToolTip.text: multiplierLabel.tooltipText

            // GridCalculator ignores such a multiplier
            customErrorMsg: {
              const value = multiplierEdit.backend.value;
              return isNaN(value) || (value > 0 && isFinite(value))
                ? "" : qsTr("The multiplier must be a positive number");
            }
          }
        }
      }

      LinesBox {
        title: qsTr("Grid")
        graphSettings: root.graphSettings
        lines: "grid"
      }

      LinesBox {
        title: qsTr("Sub-grid")
        graphSettings: root.graphSettings
        lines: "subgrid"
      }
    }
  }
}
