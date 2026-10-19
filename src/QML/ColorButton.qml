/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Shapes
import QtQuick.Dialogs

Shape {
  id: disk

  property themedColor selectedColor

  signal selectedColorModified()

  // a disk has no text, so call sites set the hint with ToolTip.text, which
  // screen readers read too
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: shape_path.hovered && ToolTip.text.length !== 0

  Accessible.role: Accessible.Button
  Accessible.name: ToolTip.text
  Accessible.onPressAction: colorDialog.open()

  /// @brief selectedColor in the app's current theme
  readonly property color shownColor: ZeStyle.themed(selectedColor)

  horizontalAlignment: Shape.AlignHCenter
  verticalAlignment: Shape.AlignVCenter
  preferredRendererType: Shape.CurveRenderer

  property real radius: 10

  implicitHeight: 2*radius
  implicitWidth: 2*radius

  ShapePath {
    id: shape_path
    strokeWidth: -1

    property real h: disk.shownColor.hsvHue
    property real s: disk.shownColor.hsvSaturation
    property real v: disk.shownColor.hsvValue

    Behavior on h {
      NumberAnimation { duration: 500 }
    }

    Behavior on s {
      NumberAnimation { duration: 500 }
    }

    Behavior on v {
      NumberAnimation { duration: 500 }
    }

    readonly property bool hovered: mouse_area.containsMouse
      && (mouse_area.mouseX - disk.width/2)**2 + (mouse_area.mouseY - disk.height/2)**2 <= disk.radius**2

    property real deltaV: hovered ? 0.7 : 0.5

    Behavior on deltaV {
      NumberAnimation { duration: 100 }
    }

    readonly property real stop0_v: Math.min(v + deltaV, 1.0)
    readonly property real stop1_v: stop0_v - deltaV

    PathAngleArc {
      centerX: disk.radius
      centerY: disk.radius
      radiusX: disk.radius
      radiusY: disk.radius
      sweepAngle: 360
    }

    fillGradient: RadialGradient {
      centerX: disk.radius * 0.8
      centerY: disk.radius * 0.8
      centerRadius: disk.radius * 0.9
      focalX: centerX
      focalY: centerY
      GradientStop { position: 0; color: Qt.hsva(shape_path.h, shape_path.s, shape_path.stop0_v, 1.0)}
      GradientStop { position: 1; color: Qt.hsva(shape_path.h, shape_path.s, shape_path.stop1_v, 1.0) }
    }
  }

  ColorDialog {
    id: colorDialog
    selectedColor: disk.shownColor
    onAccepted: {
      disk.selectedColor.setCurrent(selectedColor);
      disk.selectedColorChanged();
      disk.selectedColorModified();
    }
  }

  MouseArea {
    id: mouse_area
    anchors.fill: parent
    hoverEnabled: true

    onReleased: colorDialog.open()
  }
}
