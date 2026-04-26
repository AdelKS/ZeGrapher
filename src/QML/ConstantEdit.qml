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
  property alias animatedConstant: wrapped_backend

  implicitHeight: mainLayout.implicitHeight

  Transition {
    id: commonTransition
    reversible: true
    from: "shown"
    to: "hidden"
    SequentialAnimation {
      NumberAnimation {
        easing.type: Easing.InOutQuad;
        property: "maximumHeight";
        duration: 200;
      }
      PropertyAction {
        property: "visible"
      }
    }
  }

  AnimatedConstant {
    id: wrapped_backend
    backend: root.backend
  }

  Connections {
    target: wrapped_backend

    function onFromChanged() {
      from.setValue(wrapped_backend.from);
    }
    function onToChanged() {
      to.setValue(wrapped_backend.to);
    }
    function onStepsChanged() {
      steps.value = wrapped_backend.steps;
    }
    function onValueChanged() {
      constant.setValue(backend.value)
    }
    function onSliderPosChanged() {
      slider.value = wrapped_backend.sliderPos
    }
    function onPlayingChanged() {
      playButton.checked = wrapped_backend.playing;
    }
  }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  ColumnLayout {
    id: mainLayout
    anchors.fill: parent

    RowLayout {

      GridLayout {
        columns: 3
        rowSpacing: 0

        ZeLabel {
          Layout.alignment: Qt.AlignHCenter
          text: qsTr("name")
        }

        Item {}

        ZeLabel {
          id: valueLabel
          Layout.alignment: Qt.AlignHCenter
          text: qsTr("value")
          enabled: !wrapped_backend.deadAndAlive
        }

        ZcExprEdit {
          id: zcExprEdit

          Layout.maximumWidth: 60

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

          enabled: !wrapped_backend.deadAndAlive
        }
      }
    }

    GridLayout {
      columns: 3
      id: sliderLayout
      rowSpacing: 0
      columnSpacing: 0
      clip: true

      ZeLabel {
        Layout.alignment: Qt.AlignHCenter
        text: qsTr("min")
      }

      ZeLabel {
        Layout.alignment: Qt.AlignHCenter
        text: qsTr("steps")
      }

      ZeLabel {
        Layout.alignment: Qt.AlignHCenter
        text: qsTr("max")
      }

      NumberEdit {
        id: from
        implicitWidth: 70

        onNumberEdited: wrapped_backend.from = value;
      }


      Item {
        id: sliderOrSteps

        Layout.fillWidth: true
        Layout.fillHeight: true

        Slider {
          id: slider
          anchors.fill: parent

          opacity: wrapped_backend.deadAndAlive ? 0 : 1
          visible: opacity > 0
          Behavior on opacity {
            SequentialAnimation {
              PauseAnimation { duration: 200 }
              NumberAnimation { duration: 200 }
            }
          }

          onMoved: {
            console.info("slider moved: ", value);
            wrapped_backend.sliderPos = value;
          }

          from: 0.
          stepSize: 0.01
          value: 0.5
          to: 1.
        }

        ZeSpinBox {
          id: steps
          anchors.centerIn: parent

          opacity: wrapped_backend.deadAndAlive ? 1 : 0
          visible: opacity > 0

          Behavior on opacity {
            SequentialAnimation {
              PauseAnimation { duration: 200 }
              NumberAnimation { duration: 200 }
            }
          }

          from: 1
          value: 5
          stepSize: 5
          to: 100

          onValueModified: wrapped_backend.steps = value
        }


        states: [
          State {
            name: "hidden";
            when: constant.textInput.text.length === 0 && !wrapped_backend.deadAndAlive
            PropertyChanges {
              sliderLayout.Layout.maximumHeight: 0
              sliderLayout.visible: false
            }
          },
          State {
            name: "shown";
            when: constant.textInput.text.length !== 0 || wrapped_backend.deadAndAlive
            PropertyChanges {
              sliderLayout.Layout.maximumHeight: sliderLayout.implicitHeight
              sliderLayout.visible: true
            }
          }
        ]

        transitions: commonTransition
      }

      NumberEdit {
        id: to
        implicitWidth: 70

        onNumberEdited: wrapped_backend.to = value;
      }
    }

    RowLayout {
      id: animationLayout
      spacing: 5
      clip: true

      Layout.alignment: Qt.AlignHCenter

      IconButton {
        id: playButton
        Layout.maximumHeight: 30
        Layout.maximumWidth: 30

        lightThemeIcon: checked ? "qrc:/icons/pause-dark.svg" : "qrc:/icons/play-dark.svg"
        darkThemeIcon: checked ? "qrc:/icons/pause-light.svg" : "qrc:/icons/play-light.svg"
        checkable: true

        onCheckedChanged: wrapped_backend.playing = checked;
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
          if(checked)
            wrapped_backend.loopType = AnimatedConstant.REPEAT;
          else if (!pingPongButton.checked)
            wrapped_backend.loopType = AnimatedConstant.ONESHOT;
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
          if(checked)
            wrapped_backend.loopType = AnimatedConstant.PING_PONG;
          else if (!loopButton.checked)
            wrapped_backend.loopType = AnimatedConstant.ONESHOT;
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

        onValueChanged: wrapped_backend.setDuration(value)
      }
    }
  }

  Connections {
    target: constant.textInput
    function onTextEdited() {
      console.info("new double value: ", constant.textInput.text);

      let value = parseFloat(constant.textInput.text);
      wrapped_backend.value = value;

      returnToCenter.start();
    }
  }

  NumberAnimation {
    id: returnToCenter
    target: slider
    property: "value"
    to: 0.5
    duration: 300
    easing.type: Easing.OutCubic
  }

  states: [
    State {
      name: "hidden";
      when: constant.textInput.text.length === 0 || wrapped_backend.deadAndAlive
      PropertyChanges {
        animationLayout.Layout.maximumHeight: 0
        animationLayout.visible: false
      }
    },
    State {
      name: "shown";
      when: constant.textInput.text.length !== 0 && !wrapped_backend.deadAndAlive
      PropertyChanges {
        animationLayout.Layout.maximumHeight: animationLayout.implicitHeight
        animationLayout.visible: true
      }
    }
  ]

  transitions: commonTransition


  Component.onCompleted: {
    console.debug("backendEdit: backend=", backend);
    AnimationConductor.track(wrapped_backend);
  }

  Component.onDestruction: {
    AnimationConductor.untrack(wrapped_backend);
  }

}
