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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root
  clip: true

  implicitHeight: layout.implicitHeight + 2 * layout.anchors.margins
  implicitWidth: layout.implicitWidth + 2 * layout.anchors.margins

  property alias csvFilePath: csvFile.text
  property alias previewModel: csvPreviewModel
  property alias dataSheet: csvPreviewModel.dataSheet

  signal done()

  Behavior on height {
    NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
  }

  CsvPreviewModel {
    id: csvPreviewModel
    separator: separatorEdit.text
    rowSkipCount: skipRows.value
    maxRowCount: maxRows.value
    csvHasHeaderRow: hasHeaderRow.checked
  }

  Popup {
    popupType: Popup.Item
    visible: csvPreviewModel.loadingState !== CsvPreviewModel.FREE
    anchors.centerIn: parent
    closePolicy: Popup.NoAutoClose
    modal: true

    ColumnLayout {
      anchors.fill: parent
      ZeLabel {
        text: csvPreviewModel.loadingState === CsvPreviewModel.READING_CSV_FILE ? qsTr("Reading CSV file") : qsTr("Loading data")
      }
      ProgressBar {
        from: 0
        to: 100
        value: csvPreviewModel.progressPercentage
      }
    }
  }

  ColumnLayout {
    id: layout
    anchors.fill: parent
    anchors.margins: 5
    spacing: 10

    ZeLabel {
      text: qsTr("CSV file to load")
    }

    TextField {
      id: csvFile

      Layout.leftMargin: 5
      Layout.fillWidth: true
      readOnly: true
    }

    GroupBox {
      title: qsTr("CSV import settings")
      Layout.fillWidth: true

      RowLayout {
        anchors.fill: parent

        Item {
          Layout.fillWidth: true
          implicitWidth: 0
        }

        GridLayout {
          columns: 2
          Layout.fillWidth: true

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            id: separatorLabel
            text: qsTr("Separator:")
            // "\\t" is kept out of qsTr(), because lupdate would write a real
            // tab into the .ts file
            tooltipText: qsTr("The string that separates two cells of a row. Write %1 for a TAB separator.").arg("\\t")
          }
          LineEdit {
            Layout.alignment: Qt.AlignLeft
            id: separatorEdit
            ToolTip.text: separatorLabel.tooltipText
            Layout.preferredWidth: 30
            text: ","
            border.color: "grey"
          }
          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Header row:")
          }
          CheckBox {
            Layout.alignment: Qt.AlignCenter
            id: hasHeaderRow
            // with no text, the contentItem still has the indicator width plus
            // the spacing, so take the implicit size from the indicator itself
            implicitWidth: implicitIndicatorWidth
            implicitHeight: implicitIndicatorHeight
          }
        }

        Item {
          Layout.fillWidth: true
          implicitWidth: 0
        }

        GridLayout {
          columns: 2
          Layout.fillWidth: true

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Preview rows:")
          }
          ZeSpinBox {
            Layout.alignment: Qt.AlignLeft
            id: maxRows
            from: 1
            to: 1000
            value: 10
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Skip rows:")
          }
          ZeSpinBox {
            Layout.alignment: Qt.AlignLeft
            id: skipRows
            to: 100000
            value: 0
          }
        }

        Item {
          Layout.fillWidth: true
          implicitWidth: 0
        }
      }
    }

    GroupBox {
      Layout.fillWidth: true
      // Absorb leftover space (or, when the parent is height-capped, take
      // the deficit) so the surrounding settings + buttons keep their
      // natural sizes. The DataTable inside scrolls via its own ScrollView.
      Layout.fillHeight: true

      title: qsTr("Preview")

      DataTable {
        id: csvPreviewTable
        anchors.fill: parent

        model: csvPreviewModel
        interactive: false
      }
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.rightMargin: 5

      Item {
        Layout.fillWidth: true
      }

      Button {
        text: qsTr("Cancel")
        onReleased: root.done()
      }

      Button {
        text: qsTr("Load")
        onReleased: {
          csvPreviewModel.loadIntoWorld();
          root.done();
        }
      }
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
    }
  }
}
