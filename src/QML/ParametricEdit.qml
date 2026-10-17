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

// The pair of object names that make up a parametric equation

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property Parametric backend

  implicitHeight: layout.implicitHeight
  implicitWidth:  layout.implicitWidth + 2 * layout.spacing

  RowLayout {
    id: layout

    anchors.fill: parent
    anchors.leftMargin: spacing
    anchors.rightMargin: spacing

    spacing: 10

    NameEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      backend: root.backend.obj1
      label: qsTr("x object name")
    }

    NameEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      backend: root.backend.obj2
      label: qsTr("y object name")
    }
  }
}
