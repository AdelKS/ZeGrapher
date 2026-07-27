import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtCore

Item {
  id: root

  required property size globalMenuSize

  signal importCSV(file: url, dataSheet: DataSheet)

  property real objectsImplicitWidth: 0

  implicitWidth: objectsImplicitWidth + verticalScrollBar.width * verticalScrollBar.visible

  ListView {
    id: listView
    anchors.fill: parent
    anchors.margins: 5
    anchors.rightMargin: 0
    clip: true

    model: MathWorld
    spacing: 10

    bottomMargin: root.globalMenuSize.height + 10

    ScrollBar.vertical: ScrollBar {
      id: verticalScrollBar
      policy: listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    }

    move: Transition {
      NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
    }
    moveDisplaced: Transition {
      NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
    }

    delegate: ListViewItem {
      id: dragWrapper

      required property var modelData

      view: listView
      content: input
      dragHandle: input.dragHandle

      width: listView.width - verticalScrollBar.width * verticalScrollBar.visible

      onMoveRequested: (from, to) => MathWorld.moveMathObject(from, to)
      onRemoveRequested: MathWorld.removeMathObject(dragWrapper.modelData)

      Connections {
        target: dragWrapper.modelData
        function onInitiateUiDelete() {
          dragWrapper.collapseAndRemove();
        }
      }

      MathObjectInput {
        id: input
        mathObj: dragWrapper.modelData

        Component.onCompleted: root.objectsImplicitWidth = Math.max(root.objectsImplicitWidth, implicitWidth)
        onImplicitWidthChanged: root.objectsImplicitWidth = Math.max(root.objectsImplicitWidth, implicitWidth)

        onImportCSV: (file, dataSheet) => { root.importCSV(file, dataSheet); }
      }
    }
  }

  FileDialog {
    id: fileDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
    nameFilters: [qsTr("CSV") + " (*.csv)", qsTr("Text") + " (*.txt)", qsTr("Any") + " (*)"]
    visible: false
    onAccepted: root.importCSV(selectedFile, null)
  }

  RowLayout
  {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10 + Math.max((root.globalMenuSize.height - height) / 2., 0)
    anchors.margins: 10
    height: implicitHeight
    spacing: 5

    Item {
      Layout.fillWidth: true
      Layout.minimumWidth: root.globalMenuSize.width
    }

    IconRoundButton {
      id: importCSV
      z: 100
      Layout.preferredHeight: 35
      Layout.preferredWidth: 35
      Layout.alignment: Qt.AlignRight

      lightThemeIcon: "qrc:/icons/csv-import-dark.svg"
      darkThemeIcon: "qrc:/icons/csv-import-light.svg"

      onReleased: fileDialog.visible = true;
    }

    IconRoundButton {
      id: add
      z: 100
      Layout.preferredHeight: 35
      Layout.preferredWidth: 35
      Layout.alignment: Qt.AlignRight
      darkThemeIcon: "qrc:/icons/add-light.svg"
      lightThemeIcon: "qrc:/icons/add.svg"

      onReleased: MathWorld.addMathObject();
    }
  }
}
