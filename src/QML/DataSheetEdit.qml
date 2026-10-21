/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

// The columns of a DataSheet, listed as editable rows

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs

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

  /// @brief sets columnsImplicitWidth to the widest row the list holds
  function updateColumnsWidth() {
    let widest = 0;
    for (const child of listView.contentItem.children) {
      // ListView hides the delegates it pools for reuse
      const row = child as ListViewItem;
      if (row && row.visible)
        widest = Math.max(widest, row.content.implicitWidth);
    }
    columnsImplicitWidth = widest;
  }

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

    ReorderList {
      id: listView

      model: root.sheet
      spacing: 5
      reuseItems: true

      bottomMargin: buttonsFrame.height + 15

      onMoveRequested: (from, to) => root.sheet.moveColumn(from, to)
      onRemoveRequested: (column) => root.sheet.deleteColumn(column)

      delegate: ListViewItem {
        id: dragWrapper

        content: input
        dragHandle: input.dragHandle

        width: listView.width - root.scrollBarWidth

        onVisibleChanged: Qt.callLater(root.updateColumnsWidth)

        DataColumnEdit {
          id: input
          backend: dragWrapper.modelData

          onDeleteRequested: dragWrapper.collapseAndRemove()

          Component.onCompleted: Qt.callLater(root.updateColumnsWidth)
          onImplicitWidthChanged: Qt.callLater(root.updateColumnsWidth)
        }
      }
    }
  }

  CsvFileDialog {
    id: importDialog
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
    onAccepted: root.importCSV(selectedFile, root.sheet)
  }

  CsvFileDialog {
    id: exportDialog
    fileMode: FileDialog.SaveFile
    defaultSuffix: "csv"
    onAccepted: root.sheet.exportCSV(selectedFile)
  }

  ButtonTray {
    id: buttonsFrame

    anchors.right: parent.right
    anchors.bottom: resizeHandle.top
    anchors.margins: 5

    height: implicitHeight
    width: implicitWidth

    RowLayout {
      id: buttons
      spacing: 5

      IconRoundButton {
        id: importColumns
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        iconName: "csv-import"

        ToolTip.text: qsTr("Load a CSV file into this sheet")

        onClicked: importDialog.visible = true;
      }

      IconRoundButton {
        id: exportColumns
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        iconName: "csv-export"

        ToolTip.text: qsTr("Write the columns of this sheet to a CSV file")

        onClicked: exportDialog.visible = true;
      }

      IconRoundButton {
        id: addColumn
        Layout.preferredHeight: 30
        Layout.preferredWidth: 30

        iconName: "add"

        ToolTip.text: qsTr("Add a column")

        onClicked: root.sheet.addColumn();
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
      color: resizeHandle.pressed || resizeHandle.containsMouse ? ZeStyle.palette.highlight : ZeStyle.palette.mid
    }
  }
}
