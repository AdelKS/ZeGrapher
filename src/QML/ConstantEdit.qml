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
      clip: true

      Layout.maximumHeight: sliderMaxHeight

      property int sliderMaxHeight: implicitHeight

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

    RowLayout {
      id: animationLayout
      spacing: 5
      clip: true

      Layout.alignment: Qt.AlignHCenter
      Layout.maximumHeight: animationMaxHeight

      property int animationMaxHeight: implicitHeight

      IconButton {
        Layout.maximumHeight: 30
        Layout.maximumWidth: 30

        id: animateButton
        lightThemeIcon: checked ? "qrc:/icons/pause-dark.svg" : "qrc:/icons/play-dark.svg"
        darkThemeIcon: checked ? "qrc:/icons/pause-light.svg" : "qrc:/icons/play-light.svg"
        checkable: true
      }

      IconButton {
        Layout.maximumHeight: 30
        Layout.maximumWidth: 30

        id: loopButton
        lightThemeIcon: checked ? "qrc:/icons/loop-dark.svg" : "qrc:/icons/loop-dark.svg"
        darkThemeIcon: checked ? "qrc:/icons/loop-light.svg" : "qrc:/icons/loop-light.svg"
        checkable: true

        onCheckedChanged: {
          if (checked && pingPongButton.checked)
            pingPongButton.checked = false;
        }
      }

      IconButton {
        Layout.maximumHeight: 30
        Layout.maximumWidth: 30

        id: pingPongButton
        lightThemeIcon: checked ? "qrc:/icons/ping-pong-dark.svg" : "qrc:/icons/ping-pong-dark.svg"
        darkThemeIcon: checked ? "qrc:/icons/ping-pong-light.svg" : "qrc:/icons/ping-pong-light.svg"
        checkable: true

        onCheckedChanged: {
          if (checked && loopButton.checked)
            loopButton.checked = false;
        }
      }

      ZeDoubleSpinBox {

        id: animationPeriod
        suffix: qsTr('s') // seconds
        decimals: 2
        from: 0.1
        to: 500.
        step: 1.

        Component.onCompleted: setValue(5.0)
      }
    }
  }

  states: [
    State {
      name: "hidden";
      when: constant.textInput.text.length === 0
      PropertyChanges {
        sliderLayout.sliderMaxHeight: 0
        sliderLayout.visible: false

        animationLayout.animationMaxHeight: 0
        animationLayout.visible: false
      }
    },
    State {
      name: "shown";
      when: constant.textInput.text.length !== 0
      PropertyChanges {
        sliderLayout.sliderMaxHeight: sliderLayout.implicitHeight
        sliderLayout.visible: true

        animationLayout.animationMaxHeight: animationLayout.implicitHeight
        animationLayout.visible: true
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
        property: "animationMaxHeight";
        duration: 200;
      }
      NumberAnimation {
        easing.type: Easing.InOutQuad;
        property: "sliderMaxHeight";
        duration: 200;
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
