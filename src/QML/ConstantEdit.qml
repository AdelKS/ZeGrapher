// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root
  clip: true

  required property Constant backend

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight
  property alias animatedConstant: wrapped_backend

  onImplicitHeightChanged: {
    console.info("ConstantEdit: implicitHeight: ", implicitHeight);
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

   GridLayout {
    id: nameLayout

    columns: 2
    rowSpacing: 0

    anchors.left: parent.left
    anchors.top: parent.top

    ZeLabel {
      Layout.alignment: Qt.AlignHCenter
      text: qsTr("name")
    }

    Item {}

    ZcExprEdit {
      id: zcExprEdit

      Layout.preferredWidth: 60

      state: root.backend.state
      onTextEdited: root.backend.setName(expression)

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter
    }

    ZeLabel {
      text: "="
      Layout.preferredWidth: 20
      Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
    }
  }

  ColumnLayout {
    id: mainLayout
    anchors.left: parent.left
    anchors.right: parent.right
    y: 0

    ColumnLayout {
      id: valueLayout

      spacing: 0
      clip: true
      Layout.leftMargin: nameLayout.width
      Layout.fillWidth: true

      ZeLabel {
        id: valueLabel
        Layout.alignment: Qt.AlignHCenter
        text: qsTr("value")
      }

      NumberEdit
      {
        id: constant

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
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
        id: stepsLabel
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
          enabled: opacity > 0.5

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
          width: Math.min(implicitWidth, parent.width)
          enabled: opacity > 0.5

          from: 1
          value: 5
          stepSize: 5
          to: 100

          onValueModified: wrapped_backend.steps = value
        }
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
      name: "empty"
      when: !wrapped_backend.deadAndAlive && constant.textInput.text.length === 0
      PropertyChanges {
        mainLayout.y: 0
        valueLayout.Layout.maximumHeight: valueLayout.implicitHeight
        valueLayout.opacity: 1.

        sliderLayout.columnSpacing: 0
        sliderLayout.Layout.leftMargin: 0
        sliderLayout.Layout.maximumHeight: 0

        animationLayout.Layout.maximumHeight: 0

        slider.opacity: 1
        steps.opacity: 0

        stepsLabel.opacity: 0.

        root.implicitHeight: valueLayout.implicitHeight
      }
    },
    State {
      name: "numeric"
      when: !wrapped_backend.deadAndAlive && constant.textInput.text.length !== 0
      PropertyChanges {
        mainLayout.y: 0
        valueLayout.Layout.maximumHeight: valueLayout.implicitHeight
        valueLayout.opacity: 1.

        sliderLayout.columnSpacing: 0
        sliderLayout.Layout.leftMargin: 0
        sliderLayout.Layout.maximumHeight: sliderLayout.implicitHeight

        animationLayout.Layout.maximumHeight: animationLayout.implicitHeight

        slider.opacity: 1
        steps.opacity: 0
        stepsLabel.opacity: 0.

        root.implicitHeight: valueLayout.implicitHeight + sliderLayout.implicitHeight + animationLayout.implicitHeight + 2 * mainLayout.spacing
      }
    },
    State {
      name: "schrodinger"
      when: wrapped_backend.deadAndAlive
      PropertyChanges {
        mainLayout.y: - valueLayout.implicitHeight - mainLayout.spacing
        valueLayout.Layout.maximumHeight: valueLayout.implicitHeight
        valueLayout.opacity: 0

        sliderLayout.columnSpacing: 5
        sliderLayout.Layout.maximumHeight: sliderLayout.implicitHeight
        sliderLayout.Layout.leftMargin: nameLayout.width

        animationLayout.Layout.maximumHeight: 0

        slider.opacity: 0.
        steps.opacity: 1.
        stepsLabel.opacity: 1.

        root.implicitHeight: sliderLayout.implicitHeight
      }
    }
  ]

  transitions: Transition {
    NumberAnimation {
      properties: "implicitHeight,maximumHeight,leftMargin,opacity,y,columnSpacing"
      duration: 400
      easing.type: Easing.InOutQuad
    }
  }


  Component.onCompleted: {
    console.debug("backendEdit: backend=", backend);
    AnimationConductor.track(wrapped_backend);
  }

  Component.onDestruction: {
    AnimationConductor.untrack(wrapped_backend);
    AnimationConductor.unsetSchrodingerConstant(wrapped_backend);
  }

}
