import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root
  signal deleteMe()

  required property MathObject mathObj

  property int deleteDuration: 250

  implicitWidth: frame.implicitWidth + del.height / 2
  implicitHeight: frame.implicitHeight + del.height / 2

  onImplicitWidthChanged: {
    console.debug("MathObjectInput: implicitWidth", implicitWidth);
  }

  onImplicitHeightChanged: {
    console.debug("MathObjectInput: implicitHeight", implicitHeight);
  }

  onWidthChanged: {
    console.debug("MathObjectInput: width", width);
  }

  onHeightChanged: {
    console.debug("MathObjectInput: height", height);
  }

  function removeObj() {
    state = "deleted";
  }

  states: State {
    name: "deleted"
    PropertyChanges {
      root.opacity: 0
      root.implicitHeight: 0
    }
  }

  transitions: Transition {
    to: "deleted"
    SequentialAnimation {
      NumberAnimation {
        properties: "opacity,implicitHeight"
        duration: root.deleteDuration
        easing.type: Easing.InOutQuad
      }
      ScriptAction { script: root.deleteMe() }
    }
  }

  Frame {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: del.width / 2
    anchors.top: parent.top
    anchors.topMargin: del.height / 2
    anchors.bottom: parent.bottom

    leftPadding: 10
    rightPadding: 10
    topPadding: 5
    bottomPadding: 10

    ColumnLayout {
      id: layout
      anchors.fill: parent
      spacing: 5

      RowLayout {
        id: firstRow
        Layout.fillWidth: true
        Layout.preferredHeight: 40

        ComboBox {
          id: objectTypeTumbler
          implicitContentWidthPolicy: ComboBox.WidestText
          textRole: "text"
          valueRole: "type"
          background.implicitWidth: implicitContentWidth

          model: ListModel {
            id: eqTypeModel
            ListElement {
              text: "Equation"
              type: MathObject.EQUATION
              has_graph: true
            }
            ListElement {
              text: "Constant"
              type: MathObject.CONSTANT
              has_graph: false
            }
            ListElement {
              text: "Parametric"
              type: MathObject.PARAMETRIC
              has_graph: true
            }
            ListElement {
              text: "Data"
              type: MathObject.DATA
              has_graph: true
            }
          }

          Connections {
            target: mathObj
            function onTypeChanged() {
              objectTypeTumbler.sync();
            }
          }

          function sync() {
            let currentType = eqTypeModel.get(objectTypeTumbler.currentIndex).type
            if (mathObj.type !== currentType)
            {
              for (var i = 0; i != eqTypeModel.count; i++) {
                if (mathObj.type === model.get(i).type) {
                  currentIndex = i;
                  break;
                }
              }
            }
          }

          onActivated: {
            let newType = eqTypeModel.get(objectTypeTumbler.currentIndex).type
            if (mathObj.type !== newType)
              mathObj.type = newType;
          }
        }

        Item {
          Layout.fillWidth: true
        }

        IconRoundButton {
          id: displayButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          checkable: true
          checked: false

          lightThemeIcon: checked ? "qrc:/icons/closed-eye.svg" : "qrc:/icons/open-eye.svg"
          darkThemeIcon: checked ? "qrc:/icons/closed-eye-light.svg" : "qrc:/icons/open-eye-light.svg"
        }

        IconRoundButton {
          id: styleButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          checkable: true
          checked: false

          lightThemeIcon: "qrc:/icons/brush.svg"
          darkThemeIcon: "qrc:/icons/brush-light.svg"
        }

        ColorButton {
          id: colorButton
          radius: 12
          selectedColor: styleWidget.dataBackend.color

          onSelectedColorChanged: {
            styleWidget.dataBackend.color = selectedColor;
          }
        }

        ColorButton {
          id: secondColorButton
          radius: 12
          selectedColor: styleWidget.dataBackend.secondColor

          onSelectedColorChanged: {
            styleWidget.dataBackend.secondColor = selectedColor;
          }

          states: [
            State {
              name: "hidden";
              when: !root.mathObj.schrodinger || root.mathObj.type === MathObject.CONSTANT
              PropertyChanges {
                secondColorButton.opacity: 0.
                secondColorButton.radius: 0
                secondColorButton.visible: false
              }
            },
            State {
              name: "shown";
              when: root.mathObj.schrodinger && root.mathObj.type !== MathObject.CONSTANT
              PropertyChanges {
                secondColorButton.opacity: 1.
                secondColorButton.radius: 12
                secondColorButton.visible: true
              }
            }
          ]

          transitions: commonTransition
        }

        Item {
          Layout.fillWidth: true
        }

        Transition {
          id: commonTransition

          reversible: true
          from: "shown"
          to: "hidden"
          SequentialAnimation {
            NumberAnimation {
              easing.type: Easing.InOutQuad
              properties: "opacity,maximumWidth,maximumHeight,radius"
              duration: 200;
            }
            PropertyAction {
              property: "visible"
            }
          }
        }

        Connections {
          target: root.mathObj

          function onSchrodingerChanged() {
            if (loader.item && loader.item.animatedConstant)
            {
              loader.item.animatedConstant.deadAndAlive = root.mathObj.schrodinger;
              schrodingerCatButton.checked = root.mathObj.schrodinger;
            }
          }
        }

        IconRoundButton {
          id: schrodingerCatButton

          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          readonly property bool isConstant: root.mathObj.type === MathObject.CONSTANT

          checkable: true

          lightThemeIcon: "qrc:/icons/schrodinger-cat-dark.svg"
          darkThemeIcon: "qrc:/icons/schrodinger-cat-light.svg"

          onToggled: {
            if (checked) {
              MathWorld.setSchrodingerConstant(root.mathObj);
              AnimationConductor.setSchrodingerConstant(loader.item.animatedConstant);
            } else {
              MathWorld.unsetSchrodingerConstant(root.mathObj);
              AnimationConductor.unsetSchrodingerConstant(loader.item.animatedConstant);
            }
          }

          states: [
            State {
              name: "hidden";
              when: !schrodingerCatButton.isConstant
              PropertyChanges {
                schrodingerCatButton.opacity: 0.
                schrodingerCatButton.Layout.maximumWidth: 0
                schrodingerCatButton.visible: false
              }
            },
            State {
              name: "shown";
              when: schrodingerCatButton.isConstant
              PropertyChanges {
                schrodingerCatButton.opacity: 1.
                schrodingerCatButton.Layout.maximumWidth: 30
                schrodingerCatButton.visible: true
              }
            }
          ]

          transitions: commonTransition
        }

        Image {
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/schrodinger-cat-dark.svg" : "qrc:/icons/schrodinger-cat-light.svg"
          fillMode: Image.PreserveAspectFit
          mipmap: true

          id: schrodingerObject

          readonly property bool isSchrodingerObject: false

          states: [
            State {
              name: "hidden";
              when: !root.mathObj.schrodinger || root.mathObj.type === MathObject.CONSTANT
              PropertyChanges {
                schrodingerObject.Layout.maximumWidth: 0
                schrodingerObject.Layout.maximumHeight: 0
                schrodingerObject.visible: false
              }
            },
            State {
              name: "shown";
              when: root.mathObj.schrodinger && root.mathObj.type !== MathObject.CONSTANT
              PropertyChanges {
                schrodingerObject.Layout.maximumWidth: 30
                schrodingerObject.Layout.maximumHeight: 30
                schrodingerObject.visible: true
              }
            }
          ]

          transitions: commonTransition
        }

        states: [
          State {
            name: "hidden";
            when: ! eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
            PropertyChanges {
              colorButton.opacity: 0.
              colorButton.radius: 0
              colorButton.visible: false

              styleButton.opacity: 0.
              styleButton.Layout.maximumWidth: 0
              styleButton.Layout.maximumHeight: 0
              styleButton.visible: false

              displayButton.opacity: 0.
              displayButton.Layout.maximumWidth: 0
              displayButton.Layout.maximumHeight: 0
              displayButton.visible: false
            }
          },
          State {
            name: "shown";
            when: eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
            PropertyChanges {
              colorButton.opacity: 1.
              colorButton.radius: 12
              colorButton.visible: true

              styleButton.opacity: 1.
              styleButton.Layout.maximumWidth: 30
              styleButton.Layout.maximumHeight: 30
              styleButton.visible: true

              displayButton.opacity: 1.
              displayButton.Layout.maximumWidth: 30
              displayButton.Layout.maximumHeight: 30
              displayButton.visible: true
            }
          }
        ]

        transitions: commonTransition
      }

      ObjectStyle {
        id: styleWidget
        Layout.fillWidth: true
        Layout.preferredHeight: preferredHeight
        clip: true
        discrete: mathObj.discrete
        dataBackend.visible: !displayButton.checked

        property int preferredHeight: 0

        Behavior on preferredHeight { SmoothedAnimation { duration: 200 } }

        states: [
          State {
            name: "hidden"; when: !styleButton.checked || !styleButton.visible
            PropertyChanges { styleWidget.preferredHeight: 0 }
          },
          State {
            name: "shown"; when: styleButton.checked && styleButton.visible
            PropertyChanges {
              explicit: false
              styleWidget.preferredHeight: styleWidget.implicitHeight
            }
          }
        ]
      }

      Loader {
        id: loader
        Layout.fillWidth: true

        property int currentType: -1

        onStatusChanged: {
          let statusStrings = [];
          statusStrings[Loader.Null] = "Null";
          statusStrings[Loader.Ready] = "Ready";
          statusStrings[Loader.Loading] = "Loading";
          statusStrings[Loader.Error] = "Error";
          console.debug("Loader status: ", statusStrings[status]);
        }

        Connections {
          target: mathObj
          function onTypeChanged() {
            loader.sync();
          }
        }

        function sync() {
          if (currentType !== mathObj.type)
          {
            currentType = mathObj.type
            if (currentType === MathObject.EQUATION) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/EquationEdit.qml", {"backend": mathObj.getEquation()});
            } else if (currentType === MathObject.CONSTANT) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/ConstantEdit.qml", {"backend": mathObj.getConstant()});
            } else if (currentType === MathObject.PARAMETRIC) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/ParametricEdit.qml", {"backend": mathObj.getParametric()});
            } else if (currentType === MathObject.DATA) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/DataEdit.qml", {"backend": mathObj.getData()});
            }
          }
        }
      }
    }
  }


  IconRoundButton {
    id: del
    width: 15
    height: 15
    anchors.horizontalCenter: frame.right
    anchors.horizontalCenterOffset: -2
    anchors.verticalCenter: frame.top
    anchors.verticalCenterOffset: 2
    z: 1

    lightThemeIcon: "qrc:/icons/delete-dark.svg"
    darkThemeIcon: "qrc:/icons/delete-light.svg"

    onReleased: root.removeObj()
  }

  Component.onCompleted: {
    loader.sync();
    objectTypeTumbler.sync();
    MathWorld.attachStyle(mathObj, styleWidget.dataBackend);
  }

}
