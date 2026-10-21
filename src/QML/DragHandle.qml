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

// The handle to drag an object or column within its list

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  Layout.minimumWidth: 20
  Layout.maximumWidth: 30
  Layout.preferredHeight: Layout.preferredWidth
  Layout.preferredWidth: 30

  // call sites set the hint with ToolTip.text
  HoverHandler { id: hover }

  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hover.hovered && ToolTip.text.length !== 0

  Image {
    anchors.fill: parent
    source: ZeStyle.icon("drag-handle")
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
