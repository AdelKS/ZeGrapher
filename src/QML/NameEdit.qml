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

// A line edit for the name a math object is known by, with an optional label on top

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  // any backend with a 'name', a 'state' and setName(): NamedRef, Constant, Data
  required property QtObject backend

  property alias label: label.text

  // where the line edit sits, for callers that align siblings with it
  readonly property alias exprY: zcExprEdit.y
  readonly property alias exprHeight: zcExprEdit.exprHeight

  spacing: 0

  // call sites set the hint with ToolTip.text, which the label and line edit show
  ZeLabel {
    id: label

    tooltipText: root.ToolTip.text

    visible: text.length !== 0
    Layout.alignment: Qt.AlignHCenter
  }

  ZcExprEdit {
    id: zcExprEdit

    hint: root.ToolTip.text
    state: root.backend.state

    expression: root.backend.name
    onTextEdited: (expression) => root.backend.setName(expression)

    Layout.fillWidth: true
    Layout.alignment: Qt.AlignTop
  }
}
