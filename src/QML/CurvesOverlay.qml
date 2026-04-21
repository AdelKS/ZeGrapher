import QtQuick
import QtQuick.Shapes

// Renders continuous math curves as GPU-accelerated Shape items.
// Positioned at the graph rect within its parent InteractiveGraph item.
//
// 'qmlData' is a list of per-curve maps:
//   { style: CurveStyle,
//     segments:    [QPolygonF, ...],    // present when a line is drawn
//     markerPaths: [QPolygonF, ...] }   // present when pointStyle != None
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

            // Markers (Rhombus, Disc, Square, Triangle, Cross)
            Shape {
                anchors.fill: parent
                preferredRendererType: Shape.CurveRenderer
                visible: curveDelegate.modelData.markerPaths !== undefined
                         && curveDelegate.modelData.markerPaths.length > 0

                ShapePath {
                    readonly property var style: curveDelegate.modelData.style
                    // CurveStyle::PointStyle: None=0, Rhombus=1, Disc=2, Square=3, Triangle=4, Cross=5
                    readonly property bool crossMarker: style.pointStyle === 5

                    strokeColor: style.color
                    strokeWidth: crossMarker ? style.pointWidth : 1
                    fillColor:   crossMarker ? "transparent" : style.color
                    capStyle:    ShapePath.RoundCap
                    joinStyle:   ShapePath.RoundJoin

                    PathMultiline {
                        paths: curveDelegate.modelData.markerPaths ?? []
                    }
                }
            }
        }
    }
}
