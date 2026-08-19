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

      font: Information.appSettings.font
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
            // "\\t" stays out of qsTr(): lupdate would write a real tab into the .ts
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
            // Without text the contentItem still holds the indicator's width plus
            // the spacing, so the implicit size has to be taken from the indicator.
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
          SpinBox {
            Layout.alignment: Qt.AlignLeft
            id: maxRows
            value: 10
          }

          ZeLabel {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Skip rows:")
          }
          SpinBox {
            Layout.alignment: Qt.AlignLeft
            id: skipRows
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
