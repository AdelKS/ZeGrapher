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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs

Item {
  id: root

  required property size globalMenuSize

  signal importCSV(file: url, dataSheet: DataSheet)

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

      MathObjectInput {
        id: input
        mathObj: dragWrapper.modelData

        onDeleteRequested: dragWrapper.collapseAndRemove()

        onImportCSV: (file, dataSheet) => { root.importCSV(file, dataSheet); }
      }
    }
  }

  CsvFileDialog {
    id: fileDialog
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
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

    ButtonTray {
      id: buttonsFrame

      Layout.alignment: Qt.AlignRight

      RowLayout {
        spacing: 5

        IconRoundButton {
          id: importCSV
          Layout.preferredHeight: 35
          Layout.preferredWidth: 35

          iconName: "csv-import"

          ToolTip.text: qsTr("Load a CSV file into a new data sheet")

          onClicked: fileDialog.visible = true;
        }

        IconRoundButton {
          id: add
          Layout.preferredHeight: 35
          Layout.preferredWidth: 35

          iconName: "add"

          ToolTip.text: qsTr("Add a math object")

          onClicked: MathWorld.addMathObject();
        }
      }
    }
  }
}
