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

// A small question mark that shows a hint on hover or tap

import QtQuick
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property string text

  implicitWidth: 16
  implicitHeight: 16

  Accessible.role: Accessible.StaticText
  Accessible.name: root.text

  Label {
    anchors.centerIn: parent
    text: "?"
    font.pixelSize: 11
    font.bold: true
    opacity: 0.7
  }

  HoverHandler {
    id: hover
  }

  // a touchscreen has no hover. A mouse click would keep the hint open after
  // the pointer leaves
  TapHandler {
    id: tap
    acceptedDevices: PointerDevice.TouchScreen
    property bool shown: false
    onTapped: shown = !shown
  }

  // its own ToolTip, not the attached one: the attached ToolTip is shared, and
  // its width is the unwrapped text width, so a long hint spans the window
  ToolTip {
    text: root.text
    delay: hover.hovered ? ZeStyle.tooltipDelay : 0
    visible: hover.hovered || tap.shown
    width: Math.min(implicitWidth, 320)

    // the ToolTip closes on a press outside: reset shown, so the next tap opens it
    onClosed: tap.shown = false
  }
}
