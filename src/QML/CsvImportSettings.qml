import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root
  clip: true

  implicitHeight: layout.implicitHeight
  implicitWidth: layout.implicitWidth

  property alias csvFile: csvFile.text
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

  ColumnLayout {
    id: layout
    spacing: 10

    anchors.fill: parent

    Label {
      text: qsTr("CSV file to load")
    }

    TextEdit {
      id: csvFile
      Layout.leftMargin: 10
      readOnly: true

      color: root.palette.text
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
