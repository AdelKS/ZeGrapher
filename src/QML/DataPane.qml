import QtQuick

Item {
  id: root

  property bool importingCSV: false

  function loadCSV(csvFile: url) {
    csvImportSettings.csvFilePath = csvFile.toString();
    csvImportSettings.previewModel.csvFile = csvFile;
    importingCSV = true;
  }

  CsvImportSettings {
    id: csvImportSettings

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    onDone: {
      root.importingCSV = false;
    }
  }

  DataTable {
    id: dataTable

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: 5

    model: DataTableModel
    interactive: true
    enabled: !root.importingCSV
  }

  states: [
    State {
      name: "hidden";
      when: !root.importingCSV
      PropertyChanges {
        csvImportSettings.height: 0
        csvImportSettings.visible: false
      }
      AnchorChanges {
        target: dataTable
        anchors.top: root.top
      }
    },
    State {
      name: "shown";
      when: root.importingCSV
      PropertyChanges {
        csvImportSettings.height: Math.min(csvImportSettings.implicitHeight, parent.height)
        csvImportSettings.visible: true
      }
      AnchorChanges {
        target: dataTable
        anchors.top: csvImportSettings.bottom
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
        property: "height";
        duration: 400;
      }
      AnchorAnimation {
        duration: 400
        easing.type: Easing.InOutQuad;
      }
      PropertyAction {
        property: "visible"
      }
    }
  }
}
