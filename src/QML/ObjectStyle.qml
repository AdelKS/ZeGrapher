import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// A line edit without borders, that follows its content

Item {
  id: root

  property bool showPointSettings: true

  RowLayout {
    id: rowLayout
    anchors.fill: parent

    ColumnLayout {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      ImageTumbler {
        Layout.alignment: Qt.AlignHCenter
        model: lineStyle
      }
      SpinBox {
        Layout.alignment: Qt.AlignHCenter
        editable: true
        from: 1
        to: 100
        live: true
        value: 10
        stepSize: 10

        // workaround for value not being set on initialization
        Component.onCompleted: { value = 10 }
      }
    }

    ColumnLayout {
      visible: showPointSettings
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        Layout.alignment: Qt.AlignHCenter
        model: pointStyle
        fillMode: Image.PreserveAspectFit
        maxWidth: 18
      }
      SpinBox {
        Layout.alignment: Qt.AlignHCenter
        editable: true
        from: 1
        to: 100
        live: true
        value: 10
        stepSize: 10

        Component.onCompleted: { value = 10 }
      }
    }

    ListModel {
      id: lineStyle
      ListElement {
        path: "qrc:/icons/solidLine.png"
      }
      ListElement {
        path: "qrc:/icons/dashLine.png"
      }
      ListElement {
        path: "qrc:/icons/dashDotLine.png"
      }
      ListElement {
        path: "qrc:/icons/noLine.png"
      }
      ListElement {
        path: "qrc:/icons/dotLine.png"
      }
    }

    ListModel {
      id: pointStyle

      ListElement {
        path: "qrc:/icons/squarePoint.png"
      }
      ListElement {
        path: "qrc:/icons/trianglePoint.png"
      }
      ListElement {
        path: "qrc:/icons/roundPoint.png"
      }
      ListElement {
        path: "qrc:/icons/noPoint.png"
      }
      ListElement {
        path: "qrc:/icons/crossPoint.png"
      }
      ListElement {
        path: "qrc:/icons/rhombusPoint.png"
      }
    }
  }

}
