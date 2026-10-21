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

  function loadCSV(csvFile: url, dataSheet: DataSheet) {
    csvImportSettings.dataSheet = dataSheet;
    csvImportSettings.csvFilePath = csvImportSettings.previewModel.displayPath(csvFile);
    csvImportSettings.previewModel.csvFile = csvFile;
    importingCSV = true;
  }

  CsvImportSettings {
    id: csvImportSettings

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.margins: 5

    // 0 when hidden, and never taller than the pane. The animation is the
    // `Behavior on height` of CsvImportSettings
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
