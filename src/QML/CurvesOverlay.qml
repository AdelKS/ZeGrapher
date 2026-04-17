import QtQuick
import QtQuick.Shapes

// Renders continuous math curves as GPU-accelerated Shape items.
// Positioned at the graph rect within its parent InteractiveGraph item.
//
// 'qmlData' is a list of per-curve maps:
//   { color, lineWidth, dashPattern, segments: [QPolygonF, ...] }
// built by Graph::updateQmlData() after each sampler pass.
Item {
    id: root
    clip: true

    property var qmlData: []

    Repeater {
        model: root.qmlData

        // One delegate per curve — holds the shared style properties.
        delegate: Item {
            id: curveDelegate
            required property var modelData

            anchors.fill: parent

            Repeater {
                model: curveDelegate.modelData.segments

                Shape {
                    required property var modelData  // QPolygonF for this segment

                    anchors.fill: parent
                    preferredRendererType: Shape.CurveRenderer

                    ShapePath {
                        readonly property var style: curveDelegate.modelData.style

                        strokeColor:  style.color
                        strokeWidth:  style.lineWidth
                        fillColor:    "transparent"
                        capStyle:     ShapePath.RoundCap
                        joinStyle:    ShapePath.RoundJoin
                        strokeStyle:  style.dashPattern.length > 0
                                      ? ShapePath.DashLine
                                      : ShapePath.SolidLine
                        dashPattern:  style.dashPattern

                        PathPolyline {
                            path: modelData
                        }
                    }
                }
            }
        }
    }
}
