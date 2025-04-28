import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

// A line edit without borders, that follows its content

Item {
  id: root

  property double lineWidth: Number(lineWidthSpinBox.value) / 10.0
  property int lineStyle: lineStyleModel.get(lineStyleTumbler.currentIndex).type

  property double pointWidth: Number(pointWidthSpinBox.value) / 10.0
  property int pointStyle: pointStyleModel.get(pointStyleTumbler.currentIndex).type
  property int coordinateSystem: coordinates.currentValue
  property bool continuous: repr.currentValue
  property alias start: start.value
  property alias end: end.value
  property alias step: step.value

  property int implicitHeight: mainLayout.implicitHeight

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

        id: repr
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "continuous"
        background.implicitWidth: implicitContentWidth

        model: ListModel {
          ListElement {
            text: "Continuous"
            continuous: true
          }
          ListElement {
            text: "Discrete"
            continuous: false
          }
        }
      }
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
          when: repr.currentValue
          PropertyChanges {
            stepLabel.opacity: 0.
            step.opacity: 0.
            pointStyleTumbler.opacity: 0
            pointWidthSpinBox.opacity: 0
            stepLabel.visible: false
            step.visible: false
            pointStyleTumbler.visible: false
            pointWidthSpinBox.visible: false
          }
        },
        State {
          name: "shown";
          when: !repr.currentValue
          PropertyChanges {
            stepLabel.opacity: 1.
            step.opacity: 1.
            pointStyleTumbler.opacity: 1.
            pointWidthSpinBox.opacity: 1.
            stepLabel.visible: true
            step.visible: true
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
        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        Component.onCompleted: {
          console.log("Start label implicit size: ", implicitWidth, "x", implicitHeight)
          console.log("Start label size: ", width, "x", height)
        }

        id: startLabel
        text: qsTr("Start:")
      }

      ValueEdit {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: start
        implicitName: "Start"
        expression: "xmin"
      }

      Item {
        Layout.maximumWidth: 10
      }

      Label {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        id: endLabel
        text: qsTr("End:")
      }

      ValueEdit {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: end
        implicitName: "end"
        expression: "xmax"
      }

      Item {
        Layout.maximumWidth: 10
      }

      Label {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
        Layout.preferredWidth: implicitWidth
        Layout.preferredHeight: implicitHeight

        id: stepLabel
        text: qsTr("Step:")
      }

      ValueEdit {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.minimumWidth: 30

        id: step
        implicitName: "step"
        expression: "1"
      }
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
