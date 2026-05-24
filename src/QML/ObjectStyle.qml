import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

// A line edit without borders, that follows its content

Item {
  id: root

  required property PlotStyle backend
  required property MathObject mathObj

  implicitHeight: mainLayout.implicitHeight

  function indexFromModelValue(model: var, val: int) : int {
    for (var i = 0; i !== model.count; i++) {
      if (model.get(i).type === val)
        return i;
    }
    console.warn("Value not in model: ", val)
    return -1;
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
            type: MathObject.Cartesian
          }
          ListElement {
            text: "Polar"
            type: MathObject.Polar
          }
        }

        onCurrentValueChanged: {
          root.mathObj.coordinateSystem = currentValue
        }
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        id: dashPatternTumbler
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 100
        Layout.preferredHeight: 50
        model: Application.styleHints.colorScheme === Qt.Light ? dashPatternModel : dashPatternModelLight

        onCurrentIndexChanged: {
          backend.dashPatternType = model.get(currentIndex).type;
          backend.drawLine = model.get(currentIndex).type !== PlotStyle.NoLine;
        }
      }
      SpinBox {
        id: lineWidthSpinBox
        Layout.alignment: Qt.AlignHCenter
        editable: true
        from: 1
        to: 100
        live: true
        value: backend.lineWidth * 10.
        stepSize: 10
        Layout.maximumWidth: 100

        onValueModified: {
          backend.lineWidth = Number(value) / 10.0;
        }
      }
    }

    ColumnLayout {
      Layout.alignment: Qt.AlignHCenter

      ImageTumbler {
        id: pointStyleTumbler
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 60
        Layout.preferredHeight: 50
        model: pointStyleModel
        fillMode: Image.PreserveAspectFit

        onCurrentIndexChanged: {
          root.backend.pointStyle = pointStyleModel.get(currentIndex).type
        }
      }
      SpinBox {
        id: pointWidthSpinBox
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 100
        editable: true
        from: 1
        to: 100
        live: true
        value: root.backend.pointWidth * 4.0
        stepSize: 10

        onValueModified: {
          root.backend.pointWidth = Number(value) / 4.0;
        }
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
          when: !root.mathObj.discrete
          PropertyChanges {
            pointStyleTumbler.opacity: 0
            pointWidthSpinBox.opacity: 0
            pointStyleTumbler.visible: false
            pointWidthSpinBox.visible: false
          }
        },
        State {
          name: "shown";
          when: root.mathObj.discrete
          PropertyChanges {
            pointStyleTumbler.opacity: 1.
            pointWidthSpinBox.opacity: 1.
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

      ZeLabel {
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
        backend: root.mathObj.start

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: startEdit
      }

      Item {
        Layout.maximumWidth: 10
      }

      ZeLabel {
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
        backend: root.mathObj.end

        Behavior on width { SmoothedAnimation { duration: 500 } }

        id: endEdit
      }

      Item {
        Layout.maximumWidth: 10
      }
    }
  }

  ListModel {
    id: dashPatternModel
    ListElement {
      path: "qrc:/icons/solid-line.svg"
      type: PlotStyle.Solid
    }
    ListElement {
      path: "qrc:/icons/dash-line.svg"
      type: PlotStyle.Dash
    }
    ListElement {
      path: "qrc:/icons/dash-dot-line.svg"
      type: PlotStyle.DashDot
    }
    ListElement {
      path: "qrc:/icons/dot-line.svg"
      type: PlotStyle.Dot
    }
    ListElement {
      path: "qrc:/icons/noLine.png"
      type: PlotStyle.NoLine
    }
  }

  ListModel {
    id: dashPatternModelLight
     ListElement {
      path: "qrc:/icons/solid-line-light.svg"
      type: PlotStyle.Solid
    }
    ListElement {
      path: "qrc:/icons/dash-line-light.svg"
      type: PlotStyle.Dash
    }
    ListElement {
      path: "qrc:/icons/dash-dot-line-light.svg"
      type: PlotStyle.DashDot
    }
    ListElement {
      path: "qrc:/icons/dot-line-light.svg"
      type: PlotStyle.Dot
    }
    ListElement {
      path: "qrc:/icons/noLine.png"
      type: PlotStyle.NoLine
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
