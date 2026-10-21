/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

// The app settings, in the panel's App tab

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property size globalMenuSize

  /// @brief one row per syntax color: the ZeAppSettings property, its label
  ///        and its hint
  readonly property var syntaxColors: [
    { setting: "validSyntax", text: qsTr("Valid"), hint: qsTr("Border color of a valid expression.") },
    { setting: "warningSyntax", text: qsTr("Warning"), hint: qsTr("Border color of an expression that is valid but has no value.") },
    { setting: "invalidSyntax", text: qsTr("Invalid"), hint: qsTr("Border color of an invalid expression.") },
  ]

  FontDialog {
    id: fontDialog
    currentFont: Information.appSettings.font

    onAccepted: {
      Information.appSettings.font = selectedFont;
    }
  }

  ScrollView {
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - grid.implicitWidth)/2, 0)

    contentHeight: grid.implicitHeight + root.globalMenuSize.height

    GridLayout {
      id: grid
      anchors.centerIn: parent

      columns: 2
      columnSpacing: 10

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Language')
      }
      ComboBox {
        id: langBox

        Layout.alignment: Qt.AlignLeft
        Layout.fillWidth: true

        implicitContentWidthPolicy: ComboBox.WidestText
        background.implicitWidth: implicitContentWidth
        textRole: "text"
        valueRole: "value"

        // one entry per translation the build embedded
        model: Information.appSettings.languages

        currentIndex: langBox.model.findIndex((lang) => lang.value === Information.appSettings.language)

        onActivated: (index) => {
          Information.appSettings.language = langBox.model[index].value;
        }
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Font')
      }
      Button {
        Layout.alignment: Qt.AlignLeft
        Layout.fillWidth: true

        text: fontDialog.currentFont.family
        onClicked: fontDialog.open()
      }

      ZeLabel {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Syntax coloring')
        tooltipText: qsTr("Border color of an expression, one color per state.")
      }
      Frame {
        Layout.fillWidth: true
        GridLayout {
          columns: 2

          Repeater {
            model: root.syntaxColors

            ZeLabel {
              required property var modelData
              required property int index

              Layout.row: index
              Layout.column: 0
              Layout.alignment: Qt.AlignRight
              text: modelData.text
              tooltipText: modelData.hint
            }
          }

          Repeater {
            model: root.syntaxColors

            ColorButton {
              required property var modelData
              required property int index

              Layout.row: index
              Layout.column: 1
              Layout.alignment: Qt.AlignLeft

              ToolTip.text: modelData.hint

              selectedColor: Information.appSettings[modelData.setting]

              onSelectedColorModified: Information.appSettings[modelData.setting] = selectedColor;
            }
          }
        }
      }
    }
  }
}
