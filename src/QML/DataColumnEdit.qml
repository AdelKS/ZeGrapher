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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
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

      CardButton {
        id: displayButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: !root.backend.style.visible

        iconName: checked ? "closed-eye" : "open-eye"

        ToolTip.text: checked ? qsTr("Show the curve of this column")
                              : qsTr("Hide the curve of this column")

        onToggled: root.backend.style.visible = !checked
      }

      CardButton {
        id: styleButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: false

        iconName: "brush"

        ToolTip.text: qsTr("Plot style")
      }

      CardButton {
        id: sampleButton
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: false

        iconName: "sample"

        ToolTip.text: qsTr("Fill this column by sampling an object")
      }

      CardButton {
        id: showInTable
        Layout.alignment: Qt.AlignVCenter

        checkable: true
        checked: root.backend.showInTable
        iconName: "table"

        ToolTip.text: qsTr("Show this column in the table")

        onToggled: root.backend.showInTable = checked;
      }

      ColorButton {
        id: colorButton
        radius: 12

        Layout.alignment: Qt.AlignVCenter

        ToolTip.text: qsTr("The color of the curve")

        selectedColor: root.backend.style.color

        onSelectedColorModified: root.backend.style.color = selectedColor
      }

      ColorButton {
        radius: 12

        Layout.alignment: Qt.AlignVCenter
        visible: root.backend.schrodinger

        ToolTip.text: qsTr("End color of the gradient over the family of Schrödinger curves")

        selectedColor: root.backend.style.secondColor

        onSelectedColorModified: root.backend.style.secondColor = selectedColor
      }

      Image {
        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: 30
        Layout.preferredHeight: 30
        visible: root.backend.schrodinger

        source: ZeStyle.icon("schrodinger-cat")
        fillMode: Image.PreserveAspectFit
        mipmap: true
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

      open: styleButton.checked
    }

    SamplingEdit {
      column: root.backend

      open: sampleButton.checked

      onApplied: sampleButton.checked = false
    }
  }
}
