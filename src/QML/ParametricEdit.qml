// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  property alias backend: zcBackend

  implicitHeight: layout.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  Parametric {
    id: zcBackend
    firstName: first.expression
    firstState: first.state
    secondName: second.expression
    secondState: second.state
    continuous: continuous.currentValue
    cartesian: cartesian.currentValue
  }

  ColumnLayout {
    id: layout
    anchors.fill: parent
    spacing: 10

    RowLayout {
      id: switchesLayout

      Layout.fillWidth: true

      Item {
        Layout.fillWidth: true
      }

      ComboBox {
        id: cartesian
        flat: true
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "value"
        model: ListModel{
          ListElement {
            text: "Cartesian"
            value: true
          }
          ListElement {
            text: "Polar"
            value: false
          }
        }
        background.implicitWidth: implicitContentWidth
      }

      Item {
        Layout.fillWidth: true
      }

      ComboBox {
        id: continuous
        flat: true
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "value"
        model: ListModel{
          ListElement {
            text: "Continuous"
            value: true
          }
          ListElement {
            text: "Discrete"
            value: false
          }
        }
        background.implicitWidth: implicitContentWidth
      }

      Item {
        Layout.fillWidth: true
      }
    }

    RowLayout {
      id: firstRow

      Layout.preferredHeight: Math.max(firstLabel.implicitHeight, first.implicitHeight,
                                       secondLabel.implicitHeight, second.implicitHeight)

      Layout.fillWidth: true

      Label {
        id: firstLabel
        text: "x:"
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      ZcExprEdit {
        id: first

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }

      Label {
        id: secondLabel
        text: "y:"
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      ZcExprEdit {
        id: second

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }
    }

    RowLayout {
      id: secondRow

      Layout.preferredHeight: Math.max(startLabel.implicitHeight, start.implicitHeight,
                                       endLabel.implicitHeight, end.implicitHeight,
                                       stepLabel.implicitHeight, step.implicitHeight)

      Layout.fillWidth: true

      Label {
        id: startLabel
        text: qsTr("start:")
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      ValueEdit {
        id: start

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }

      Label {
        id: endLabel
        text: qsTr("end:")
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      ValueEdit {
        id: end

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }

      Label {
        id: stepLabel
        text: qsTr("step:")
        visible: !continuous.currentValue
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      ValueEdit {
        id: step
        visible: !continuous.currentValue
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }
    }
  }


}
