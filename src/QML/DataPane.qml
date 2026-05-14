import QtQuick

Item {
  id: root

  property bool importingCSV: false

  implicitWidth: Math.max(
    dataTable.implicitWidth + 2*dataTable.anchors.margins + 5,
    root.importingCSV ? csvImportSettings.implicitWidth + 2*dataTable.anchors.margins + 5: 0
  )
  implicitHeight: dataTable.implicitHeight + 2*dataTable.anchors.margins
    + (root.importingCSV ? csvImportSettings.implicitHeight : 0)

  onImplicitHeightChanged: console.debug("DataPane: implicitHeight=", implicitHeight)
  onImplicitWidthChanged: console.debug("DataPane: implicitWidth=", implicitWidth)

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
    anchors.margins: 5

    // 0 when hidden, capped natural size when shown. The cap leaves
    // minDataTableHeight px for the data table below so both stay visible.
    // Height animation comes from `Behavior on height` inside this component.
    height: root.importingCSV ? Math.min(implicitHeight, root.height) : 0

    onDone: { root.importingCSV = false; }
  }

  DataTable {
    id: dataTable

    anchors.top: csvImportSettings.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: 5

    model: DataTableModel
    interactive: true
    enabled: !root.importingCSV
  }
}
