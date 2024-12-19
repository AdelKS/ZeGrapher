import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// A line edit without borders, that follows its content

Item {
  id: root

  property bool showPointSettings: true

  property double lineWidth: Number(lineWidthSpinBox.value) / 10.0
  property int lineStyle: lineStyleModel.get(lineStyleTumbler.currentIndex).type

  property double pointWidth: Number(pointWidthSpinBox.value) / 10.0
  property int pointStyle: pointStyleModel.get(pointStyleTumbler.currentIndex).type

  RowLayout {
    id: rowLayout
    anchors.fill: parent

    ColumnLayout {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      ImageTumbler {
        id: lineStyleTumbler
        Layout.alignment: Qt.AlignHCenter
        model: lineStyleModel
      }
      SpinBox {
        id: lineWidthSpinBox
        Layout.alignment: Qt.AlignHCenter
        editable: true
        from: 1
        to: 100
        live: true
        value: 20
        stepSize: 10

        // workaround for value not being set on initialization
        Component.onCompleted: { value = 20 }
      }
    }

    ColumnLayout {
      visible: showPointSettings
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        id: pointStyleTumbler
        Layout.alignment: Qt.AlignHCenter
        model: pointStyleModel
        fillMode: Image.PreserveAspectFit
        maxWidth: 18
      }
      SpinBox {
        id: pointWidthSpinBox
        Layout.alignment: Qt.AlignHCenter
        editable: true
        from: 1
        to: 100
        live: true
        value: 20
        stepSize: 10

        Component.onCompleted: { value = 20 }
      }
    }

    ListModel {
      id: lineStyleModel
      ListElement {
        path: "qrc:/icons/solidLine.png"
        type: Qt.SolidLine
      }
      ListElement {
        path: "qrc:/icons/dashLine.png"
        type: Qt.DashLine
      }
      ListElement {
        path: "qrc:/icons/dashDotLine.png"
        type: Qt.DashDotLine
      }
      ListElement {
        path: "qrc:/icons/noLine.png"
        type: Qt.NoPen
      }
      ListElement {
        path: "qrc:/icons/dotLine.png"
        type: Qt.DotLine
      }
    }

    ListModel {
      id: pointStyleModel

      ListElement {
        path: "qrc:/icons/squarePoint.png"
        type: PlotStyle.Square
      }
      ListElement {
        path: "qrc:/icons/trianglePoint.png"
        type: PlotStyle.Triangle
      }
      ListElement {
        path: "qrc:/icons/roundPoint.png"
        type: PlotStyle.Disc
      }
      ListElement {
        path: "qrc:/icons/noPoint.png"
        type: PlotStyle.None
      }
      ListElement {
        path: "qrc:/icons/crossPoint.png"
        type: PlotStyle.Cross
      }
      ListElement {
        path: "qrc:/icons/rhombusPoint.png"
        type: PlotStyle.Rhombus
      }
    }
  }

}
