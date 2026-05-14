import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root
  clip: true

  implicitHeight: layout.implicitHeight + 2 * layout.anchors.margins
  implicitWidth: layout.implicitWidth + 2 * layout.anchors.margins

  onImplicitWidthChanged: {
    console.debug("CsvImportSettings: implicit width: ", implicitWidth);
  }

  onImplicitHeightChanged: {
    console.debug("CsvImportSettings: implicit height: ", implicitHeight);
  }

  property alias csvFilePath: csvFile.text
  property alias previewModel: csvPreviewModel

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
        text: csvPreviewModel.loadingState === CsvPreviewModel.READING_CSV_FILE ? qsTr("Reading CSV File") : qsTr("Loading data into world")
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

    LineEdit {
      id: csvFile
      Layout.leftMargin: 5
      Layout.fillWidth: true
    }

    GroupBox {
      title: qsTr("CSV import settings")
      Layout.fillWidth: true

      GridLayout {
        anchors.fill: parent
        columns: 2

        RowLayout {
          ZeLabel {
            text: qsTr("Separator:")
          }
          LineEdit {
            id: separatorEdit
            Layout.preferredWidth: 30
            text: ","
            border.color: "grey"
          }
        }
        RowLayout {
          ZeLabel {
            text: qsTr("Skip rows:")
          }
          SpinBox {
            id: skipRows
            value: 0
          }
        }
        CheckBox {
          id: hasHeaderRow
          text: "Header row"
        }
        RowLayout {
          ZeLabel {
            text: qsTr("Preview rows:")
          }
          SpinBox {
            id: maxRows
            value: 10
          }
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
