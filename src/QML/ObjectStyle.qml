import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

import ZeGrapher as ZG

// The plot style settings of an object, unfolding and folding back on demand

Foldable {
  id: root

  required property Base base
  required property PlotStyle style

  readonly property bool available: root.base !== null && root.style !== null

  active: available && (open || shownHeight > 0)

  sourceComponent: Item {
    implicitHeight: mainLayout.implicitHeight

    readonly property var coordinateSystems: [
      //: cartesian coordinates
      { text: qsTr("Cartesian"), type: ZG.Base.Cartesian },
      //: polar coordinates
      { text: qsTr("Polar"), type: ZG.Base.Polar },
    ]

    readonly property var lineStyles: [
      { icon: "solid-line", type: PlotStyle.Solid },
      { icon: "dash-line", type: PlotStyle.Dash },
      { icon: "dash-dot-line", type: PlotStyle.DashDot },
      { icon: "dot-line", type: PlotStyle.Dot },
      { type: PlotStyle.NoLine },
    ]

    readonly property var pointStyles: [
      { icon: "square-point", type: PlotStyle.Square },
      { icon: "triangle-point", type: PlotStyle.Triangle },
      { icon: "disc-point", type: PlotStyle.Disc },
      { type: PlotStyle.None },
      { icon: "cross-point", type: PlotStyle.Cross },
      { icon: "rhombus-point", type: PlotStyle.Rhombus },
    ]

    GridLayout {
      id: mainLayout
      columns: 3
      columnSpacing: 10
      anchors.fill: parent

      ColumnLayout {
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

          model: coordinateSystems

          currentIndex: model.findIndex((entry) => entry.type === root.base.coordinateSystem)

          onActivated: root.base.coordinateSystem = currentValue
        }
      }

      ColumnLayout {
        Layout.alignment: Qt.AlignHCenter

        ImageTumbler {
          id: dashPatternTumbler
          Layout.alignment: Qt.AlignHCenter
          Layout.maximumWidth: 100
          Layout.preferredHeight: 50
          model: lineStyles

          currentIndex: model.findIndex((entry) => entry.type === root.style.lineStyle)

          onCurrentIndexChanged: root.style.lineStyle = model[currentIndex].type
        }

        ZeSpinBox {
          id: lineWidthSpinBox
          Layout.alignment: Qt.AlignHCenter
          from: 1
          to: 100
          live: true
          value: root.style.lineWidth * 10.
          stepSize: 10
          Layout.maximumWidth: 100

          ToolTip.text: qsTr("The width of the line")

          onValueModified: {
            root.style.lineWidth = Number(value) / 10.0;
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
          model: pointStyles
          fillMode: Image.PreserveAspectFit

          currentIndex: model.findIndex((entry) => entry.type === root.style.pointStyle)

          onCurrentIndexChanged: root.style.pointStyle = model[currentIndex].type
        }
        ZeSpinBox {
          id: pointWidthSpinBox
          Layout.alignment: Qt.AlignHCenter
          Layout.maximumWidth: 100
          from: 1
          to: 100
          live: true
          value: root.style.pointWidth * 4.0
          stepSize: 10

          ToolTip.text: qsTr("The size of the points")

          onValueModified: {
            root.style.pointWidth = Number(value) / 4.0;
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
            when: !root.base.discrete
            PropertyChanges {
              pointStyleTumbler.opacity: 0
              pointWidthSpinBox.opacity: 0
              pointStyleTumbler.visible: false
              pointWidthSpinBox.visible: false
            }
          },
          State {
            name: "shown";
            when: root.base.discrete
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

          id: startLabel
          text: qsTr("Start:")
          tooltipText: qsTr("The value where the plot starts.")
        }

        ValueEdit {
          Layout.alignment: Qt.AlignTop
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.minimumWidth: 30
          backend: root.base.start

          ToolTip.text: startLabel.tooltipText

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
          tooltipText: qsTr("The value where the plot ends.")
        }

        ValueEdit {
          Layout.alignment: Qt.AlignTop
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.minimumWidth: 30
          backend: root.base.end

          ToolTip.text: endLabel.tooltipText

          Behavior on width { SmoothedAnimation { duration: 500 } }

          id: endEdit
        }

        Item {
          Layout.maximumWidth: 10
        }
      }
    }
  }
}
