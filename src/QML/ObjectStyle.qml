import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

// A line edit without borders, that follows its content

Item {
  id: root
  property bool discrete: false
  property alias dataBackend: backend

  property int implicitHeight: mainLayout.implicitHeight

  onDiscreteChanged: {
    console.debug('ObjectStyle: discrete changed: ', discrete);
    if (discrete) {
      // last element of the lineStyleModel is no line
      lineStyleTumbler.currentIndex = lineStyleModel.count  - 1;
    } else {
      lineStyleTumbler.currentIndex = 0;
    }
  }

  PlotStyle {
    id: backend
    lineStyle: lineStyleModel.get(lineStyleTumbler.currentIndex).type
    lineWidth: Number(lineWidthSpinBox.value) / 10.0
    pointStyle: pointStyleModel.get(pointStyleTumbler.currentIndex).type
    pointWidth: Number(pointWidthSpinBox.value) / 10.0
    coordinateSystem: coordinates.currentValue

    onCoordinateSystemChanged: {
      if (coordinateSystem === PlotStyle.Cartesian) {
        startEdit.expression = "xmin";
        endEdit.expression = "xmax";
        stepEdit.expression = "1";
      } else if (coordinateSystem === PlotStyle.Polar) {
        startEdit.expression = "0";
        endEdit.expression = "2*math::pi";
        stepEdit.expression = "math::pi/12";
      }
    }

  }

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  GridLayout {
    id: mainLayout
    columns: 3
    columnSpacing: 10
    anchors.fill: parent

    ColumnLayout {
      // Layout.fillHeight: true
      Layout.alignment: Qt.AlignHCenter
      Layout.maximumWidth: implicitWidth
      Layout.preferredHeight: implicitHeight

      ComboBox {
        Layout.fillWidth: true
        Layout.minimumWidth: implicitWidth

        id: coordinates
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "type"
        background.implicitWidth: implicitContentWidth

        model: ListModel {
          ListElement {
            text: "Cartesian"
            type: PlotStyle.Cartesian
          }
          ListElement {
            text: "Polar"
            type: PlotStyle.Polar
          }
        }
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        id: lineStyleTumbler
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 100
        Layout.preferredHeight: 50
        model: Application.styleHints.colorScheme === Qt.Light ? lineStyleModel : lineStyleModelLight
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
        Layout.maximumWidth: 100

        // workaround for value not being set on initialization
        Component.onCompleted: { value = 20 }
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        id: pointStyleTumbler
        Layout.alignment: Qt.AlignHCenter
        model: pointStyleModel
        fillMode: Image.PreserveAspectFit
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

   RowLayout {
      Layout.columnSpan: 3
      Layout.fillWidth: true
      Layout.preferredHeight: implicitHeight
      Layout.minimumHeight: implicitHeight


      states: [
        State {
          name: "hidden";
          when: !root.discrete
          PropertyChanges {
            stepLabel.opacity: 0.
            stepEdit.opacity: 0.
            pointStyleTumbler.opacity: 0
            pointWidthSpinBox.opacity: 0
            stepLabel.visible: false
            stepEdit.visible: false
            pointStyleTumbler.visible: false
            pointWidthSpinBox.visible: false
          }
        },
        State {
          name: "shown";
          when: root.discrete
          PropertyChanges {
            stepLabel.opacity: 1.
            stepEdit.opacity: 1.
            pointStyleTumbler.opacity: 1.
            pointWidthSpinBox.opacity: 1.
            stepLabel.visible: true
            stepEdit.visible: true
            pointStyleTumbler.visible: true
            pointWidthSpinBox.visible: true
          }
        }
      ]

      transitions: Transition {
        reversible: true
        from: "shown"
        to: "hidden"
        SequentialAnimation {
          NumberAnimation {
            easing.type: Easing.InOutQuad;
            property: "opacity";
            duration: 200;
          }
          PropertyAction {
            property: "visible"
          }
        }
      }

      Label {
        Layout.topMargin: 5
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        Component.onCompleted: {
          console.debug("Start label implicit size: ", implicitWidth, "x", implicitHeight)
          console.debug("Start label size: ", width, "x", height)
        }

        id: startLabel
        text: qsTr("Start:")
      }

      ValueEdit {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30
        backend: backend.start

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: startEdit
        expression: "xmin"
      }

      Item {
        Layout.maximumWidth: 10
      }

      Label {
        Layout.topMargin: 5
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        id: endLabel
        text: qsTr("End:")
      }

      ValueEdit {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30
        backend: backend.end

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: endEdit
        expression: "xmax"
      }

      Item {
        Layout.maximumWidth: 10
      }

      Label {
        Layout.topMargin: 5
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        id: stepLabel
        text: qsTr("Step:")
      }

      ValueEdit {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30
        backend: backend.step

        id: stepEdit
        expression: "1"
      }
    }
  }

  ListModel {
    id: lineStyleModel
    ListElement {
      path: "qrc:/icons/solid-line.svg"
      type: Qt.SolidLine
    }
    ListElement {
      path: "qrc:/icons/dash-line.svg"
      type: Qt.DashLine
    }
    ListElement {
      path: "qrc:/icons/dash-dot-line.svg"
      type: Qt.DashDotLine
    }
    ListElement {
      path: "qrc:/icons/dot-line.svg"
      type: Qt.DotLine
    }
    ListElement {
      path: "qrc:/icons/noLine.png"
      type: Qt.NoPen
    }
  }

  ListModel {
    id: lineStyleModelLight
    ListElement {
      path: "qrc:/icons/solid-line-light.svg"
      type: Qt.SolidLine
    }
    ListElement {
      path: "qrc:/icons/dash-line-light.svg"
      type: Qt.DashLine
    }
    ListElement {
      path: "qrc:/icons/dash-dot-line-light.svg"
      type: Qt.DashDotLine
    }
    ListElement {
      path: "qrc:/icons/dot-line-light.svg"
      type: Qt.DotLine
    }
    ListElement {
      path: "qrc:/icons/noLine.png"
      type: Qt.NoPen
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
