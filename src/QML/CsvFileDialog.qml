// A file dialog for CSV files, which opens in the documents folder

import QtCore
import QtQuick
import QtQuick.Dialogs

FileDialog {
  currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
  nameFilters: ["CSV (*.csv)", qsTr("Text files") + " (*.txt)", qsTr("All files") + " (*)"]
}
