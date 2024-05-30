import QtQuick
import QtQuick.Shapes
import QtQuick.Controls
import QtQuick.Dialogs

Shape {
  id: disk

  horizontalAlignment: Shape.AlignHCenter
  verticalAlignment: Shape.AlignVCenter
  preferredRendererType: Shape.CurveRenderer

  property real radius: 10

  ShapePath {
    id: shape_path
    strokeWidth: -1

    property real h: 0
    property real s: 0
    property real v: 0

    Behavior on h {
      NumberAnimation { duration: 500 }
    }

    Behavior on s {
      NumberAnimation { duration: 500 }
    }

    Behavior on v {
      NumberAnimation { duration: 500 }
    }

    property bool hovered: false

    property real deltaV: hovered ? 0.7 : 0.5

    Behavior on deltaV {
      NumberAnimation { duration: 100 }
    }

    readonly property real stop0_v: Math.min(v + deltaV, 1.0)
    readonly property real stop1_v: stop0_v - deltaV

    function lighter(nv: real, factor: real): real {
      return Math.min( (nv === 0 ? 0.1*factor : nv * factor), 1.0);
    }

    function set_color(col: color) {
      shape_path.h = col.hsvHue;
      shape_path.s = col.hsvSaturation;
      shape_path.v = col.hsvValue;
    }

    function get_color(): color {
      return Qt.hsva(shape_path.h, shape_path.s, shape_path.v, 1.0)
    }

    PathAngleArc {
      centerX: radius
      centerY: radius
      radiusX: radius
      radiusY: radius
      sweepAngle: 360
    }

    fillGradient: RadialGradient {
      centerX: radius * 0.8
      centerY: radius * 0.8
      centerRadius: radius * 0.9
      focalX: centerX
      focalY: centerY
      GradientStop { position: 0; color: Qt.hsva(shape_path.h, shape_path.s, shape_path.stop0_v, 1.0)}
      GradientStop { position: 1; color: Qt.hsva(shape_path.h, shape_path.s, shape_path.stop1_v, 1.0) }
    }
  }

  ColorDialog {
    id: colorDialog
    selectedColor: { selectedColor = shape_path.get_color() }
    onAccepted: shape_path.set_color(selectedColor)
  }

  MouseArea {
    id: mouse_area
    anchors.fill: parent
    hoverEnabled: true

    property bool entered: false

    onPositionChanged: {
      if ((mouseX - disk.width/2)**2 + (mouseY - disk.height/2)**2 <= radius * radius)
      {
        if(!shape_path.hovered)
          shape_path.hovered = true
      }
      else if (shape_path.hovered)
        shape_path.hovered  = false
    }
    onExited: {
      if (shape_path.hovered)
        shape_path.hovered = false
    }
    onReleased: colorDialog.open()
  }
}
