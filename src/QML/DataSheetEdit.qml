// The columns of a DataSheet, listed as editable rows

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtCore

Item {
  id: root

  required property DataSheet sheet

  signal importCSV(file: url, dataSheet: DataSheet)

  /// @brief how tall the box grows on its own before the list starts scrolling
  property real maxAutoHeight: 300

  /// @brief height the user dragged the box to, zero as long as they haven't
  property real userHeight: 0

  readonly property real minimumHeight: notesRow.height + listView.bottomMargin + resizeHandle.height

  property real columnsImplicitWidth: 0

  readonly property real scrollBarWidth: scrollView.ScrollBar.vertical.visible
    ? scrollView.ScrollBar.vertical.width
    : 0

  readonly property real autoHeight: listView.contentHeight + minimumHeight

  implicitWidth: columnsImplicitWidth + scrollBarWidth
  implicitHeight: Math.max(minimumHeight, userHeight > 0 ? userHeight : Math.min(autoHeight, maxAutoHeight))

  RowLayout {
    id: notesRow
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.leftMargin: 5
    anchors.rightMargin: root.scrollBarWidth + 5
    height: implicitHeight
    spacing: 5

    ZeLabel {
      text: qsTr("Notes:")
    }

    TextField {
      id: notes
      Layout.fillWidth: true

      font: Information.appSettings.font
      placeholderText: qsTr("your comment on this sheet")

      text: root.sheet.notes
      onTextEdited: root.sheet.notes = text
    }
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.topMargin: notesRow.height
    anchors.bottomMargin: resizeHandle.height
    clip: true

    ScrollBar.vertical.policy: listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

    // a vertical ListView leaves contentWidth at -1, which the style would
    // render as a stray collapsed handle
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    ListView {
      id: listView

      model: root.sheet
      spacing: 5
      reuseItems: true

      bottomMargin: buttonsFrame.height + 15

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

        width: listView.width - root.scrollBarWidth

        onMoveRequested: (from, to) => root.sheet.moveColumn(from, to)
        onRemoveRequested: root.sheet.deleteColumn(dragWrapper.modelData)

        DataColumnEdit {
          id: input
          backend: dragWrapper.modelData

          onDeleteRequested: dragWrapper.collapseAndRemove()

          Component.onCompleted: root.columnsImplicitWidth = Math.max(root.columnsImplicitWidth, implicitWidth)
          onImplicitWidthChanged: root.columnsImplicitWidth = Math.max(root.columnsImplicitWidth, implicitWidth)
        }
      }
    }
  }

  FileDialog {
    id: importDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
    nameFilters: [qsTr("CSV") + " (*.csv)", qsTr("Text") + " (*.txt)", qsTr("Any") + " (*)"]
    visible: false
    onAccepted: root.importCSV(selectedFile, root.sheet)
  }

  FileDialog {
    id: exportDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.SaveFile
    defaultSuffix: "csv"
    nameFilters: [qsTr("CSV") + " (*.csv)", qsTr("Text") + " (*.txt)", qsTr("Any") + " (*)"]
    visible: false
    onAccepted: root.sheet.exportCSV(selectedFile)
  }

  Frame {
    id: buttonsFrame
    z: 100

    anchors.right: parent.right
    anchors.bottom: resizeHandle.top
    anchors.margins: 5

    leftPadding: 5
    rightPadding: 5
    topPadding: 5
    bottomPadding: 5

    height: implicitHeight
    width: implicitWidth

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    background: Rectangle {
      color: myPalette.light
      radius: 5
      opacity: 0.95
    }

    RowLayout {
      id: buttons
      spacing: 5

      IconRoundButton {
        id: importColumns
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        lightThemeIcon: "qrc:/icons/csv-import-dark.svg"
        darkThemeIcon: "qrc:/icons/csv-import-light.svg"

        ToolTip.text: qsTr("Load a CSV file into this sheet")

        onReleased: importDialog.visible = true;
      }

      IconRoundButton {
        id: exportColumns
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        lightThemeIcon: "qrc:/icons/csv-export-dark.svg"
        darkThemeIcon: "qrc:/icons/csv-export-light.svg"

        ToolTip.text: qsTr("Write the columns of this sheet to a CSV file")

        onReleased: exportDialog.visible = true;
      }

      IconRoundButton {
        id: addColumn
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        darkThemeIcon: "qrc:/icons/add-light.svg"
        lightThemeIcon: "qrc:/icons/add.svg"

        ToolTip.text: qsTr("Add a column")

        onReleased: root.sheet.addColumn();
      }
    }
  }

  MouseArea {
    id: resizeHandle
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: 10
    z: 100

    hoverEnabled: true
    cursorShape: Qt.SizeVerCursor

    ToolTip.delay: ZeStyle.tooltipDelay
    ToolTip.text: qsTr("Drag to resize the box of the sheet. Double-click for its default height.")
    ToolTip.visible: containsMouse

    // else the ListView we sit in flicks away with our drag
    preventStealing: true

    property real pressY: 0

    onPressed: (mouse) => pressY = mouse.y

    // we follow the box we resize, so the offset the mouse gained on us is the growth asked for
    onPositionChanged: (mouse) => {
      if (pressed)
        root.userHeight = Math.max(root.minimumHeight, root.height + mouse.y - pressY);
    }

    // back to growing with the content
    onDoubleClicked: root.userHeight = 0

    Rectangle {
      anchors.centerIn: parent
      width: 40
      height: 3
      radius: height / 2
      color: resizeHandle.pressed || resizeHandle.containsMouse ? root.palette.highlight : root.palette.mid
    }
  }
}
