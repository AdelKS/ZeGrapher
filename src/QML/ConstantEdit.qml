// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Constant backend

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: mainLayout.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  ColumnLayout {
    id: mainLayout
    anchors.fill: parent

    RowLayout {

      ZcExprEdit {
        id: zcExprEdit

        state: root.backend.state
        onTextEdited: root.backend.setName(expression)

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
      }

      ZeLabel {
        text: "="
        Layout.maximumWidth: 20
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
      }

      NumberEdit
      {
        id: constant

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

        focus: true
        textInput.validator: DoubleValidator{}
      }
    }

    RowLayout {
      id: sliderLayout
      spacing: 0

      Layout.maximumHeight: maxHeight

      property int maxHeight: implicitHeight

      NumberEdit {
        id: sliderLowerBound
        implicitWidth: 70
      }

      Slider {
        id: slider
        Layout.fillWidth: true

        onMoved: {

        }

        from: 0.
        stepSize: 0.01
        value: 0.5
        to: 1.
      }

      NumberEdit {
        id: sliderUpperBound
        implicitWidth: 70
      }
    }
  }

  states: [
    State {
      name: "hidden";
      when: constant.textInput.text.length === 0
      PropertyChanges {
        sliderLayout.maxHeight: 0
        sliderLayout.visible: false
      }
    },
    State {
      name: "shown";
      when: constant.textInput.text.length !== 0
      PropertyChanges {
        sliderLayout.maxHeight: sliderLayout.implicitHeight
        sliderLayout.visible: true
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
        property: "maxHeight";
        duration: 400;
      }
      PropertyAction {
        property: "visible"
      }
    }
  }

  Component.onCompleted: {
    console.debug("backendEdit: backend=", backend);
  }

}
