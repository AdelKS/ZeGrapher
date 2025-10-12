import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root
  clip: true

  implicitHeight: scrollView.implicitHeight
  implicitWidth: scrollView.implicitWidth

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
      Label {
        text: csvPreviewModel.loadingState === CsvPreviewModel.READING_CSV_FILE ? qsTr("Reading CSV File") : qsTr("Loading data into world")
      }
      ProgressBar {
        from: 0
        to: 100
        value: csvPreviewModel.progressPercentage
      }
    }
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    contentWidth: availableWidth

    onAvailableWidthChanged: console.debug("CSV import available width: ", availableWidth)

    ColumnLayout {
      id: layout
      anchors.fill: parent
      spacing: 10

      Label {
        Layout.leftMargin: 10
        text: qsTr("CSV file to load")
      }

      LineEdit {
        id: csvFile
        Layout.leftMargin: 20
        Layout.rightMargin: 5
        Layout.fillWidth: true
      }

      GroupBox {
        title: qsTr("CSV import settings")
        Layout.fillWidth: true

        GridLayout {
          anchors.fill: parent
          columns: 2

          RowLayout {
            Label {
              text: qsTr("Separator:")
            }
            LineEdit {
              id: separatorEdit
              Layout.preferredWidth: 30
              text: ","
              border.color: Information.appSettings.validSyntax
            }
          }
          RowLayout {
            Label {
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
            Label {
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
        Layout.fillWidth: true;

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
}
