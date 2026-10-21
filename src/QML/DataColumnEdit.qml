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

// One column of a DataSheet: the name it is known by, plus its table toggle

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

ObjectCard {
  id: root

  required property Data backend

  property alias dragHandle: dragHandle

  deleteHint: qsTr("Delete this column")

  ColumnLayout {
    anchors.fill: parent
    spacing: 5

    RowLayout {
      id: layout
      Layout.fillWidth: true
      spacing: 5

      NameEdit {
        backend: root.backend

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }

      PlotControls {
        id: plotControls
        Layout.alignment: Qt.AlignVCenter

        style: root.backend.style
        schrodinger: root.backend.schrodinger

        CardButton {
          id: sampleButton

          checkable: true
          checked: false

          iconName: "sample"

          ToolTip.text: qsTr("Fill this column by sampling an object")
        }

        CardButton {
          id: showInTable

          checkable: true
          checked: root.backend.showInTable
          iconName: "table"

          ToolTip.text: qsTr("Show this column in the table")

          onToggled: root.backend.showInTable = checked;
        }
      }

      SchrodingerBadge {
        Layout.alignment: Qt.AlignVCenter
        shown: root.backend.schrodinger
      }

      DragHandle {
        id: dragHandle
        Layout.alignment: Qt.AlignVCenter

        ToolTip.text: qsTr("Drag to change the order of the columns (in the exported CSV and in the plot)")
      }
    }

    ObjectStyle {
      base: root.backend
      style: root.backend.style

      open: plotControls.styleOpen
    }

    SamplingEdit {
      column: root.backend

      open: sampleButton.checked

      onApplied: sampleButton.checked = false
    }
  }
}
