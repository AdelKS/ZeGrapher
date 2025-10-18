import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Rectangle {
  id: root
  signal deleteMe()

  required property MathObject mathObj

  property int deleteDuration: 250

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  Behavior on height { SmoothedAnimation { duration: root.deleteDuration } }
  Behavior on implicitHeight { SmoothedAnimation { duration: root.deleteDuration } }
  Behavior on opacity { SmoothedAnimation { duration: root.deleteDuration } }

  implicitWidth: layout.implicitWidth
  implicitHeight: layout.implicitHeight

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

  Timer {
    id: deleteMeTimer
    onTriggered: deleteMe()
    interval: root.deleteDuration
    repeat: false
  }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.implicitHeight = 0;
    deleteMeTimer.start();
  }

  ColumnLayout {
    id: layout
    anchors.fill: parent
    spacing: 5

    implicitWidth: Math.max(firstRow.implicitWidth, loader.implicitWidth)
    implicitHeight: firstRow.implicitHeight + loader.implicitHeight

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

      RoundButton {
        id: displayButton
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30
        Layout.preferredHeight: Layout.preferredWidth
        Layout.preferredWidth: 30

        checkable: true
        checked: false

        icon.source: checked ? "qrc:/icons/closed-eye.svg" : "qrc:/icons/open-eye.svg"

        display: Button.IconOnly
        padding: 0
      }

      RoundButton {
        id: styleButton
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30
        Layout.preferredHeight: Layout.preferredWidth
        Layout.preferredWidth: 30

        checkable: true
        checked: false

        icon.source: "qrc:/icons/brush.svg"
        display: Button.IconOnly
        padding: 0
      }

      ColorButton {
        id: colorButton
        radius: 12
      }

      Item {
        Layout.fillWidth: true
      }

      RoundButton {
        id: del
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30
        Layout.preferredHeight: Layout.preferredWidth
        Layout.preferredWidth: 30

        icon.source: "qrc:/icons/delete.svg"

        onReleased: removeObj()

        display: Button.IconOnly
        padding: 0
      }

      states: [
        State {
          name: "hidden";
          when: ! eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
          PropertyChanges {
            colorButton.opacity: 0.
            styleButton.opacity: 0.
            displayButton.opacity: 0.
            colorButton.visible: false
            styleButton.visible: false
            displayButton.visible: false
          }
        },
        State {
          name: "shown";
          when: eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
          PropertyChanges {
            colorButton.opacity: 1.
            styleButton.opacity: 1.
            displayButton.opacity: 1.
            colorButton.visible: true
            styleButton.visible: true
            displayButton.visible: true
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
            duration: 500;
          }
          PropertyAction {
            property: "visible"
          }
        }
      }
    }

    ObjectStyle {
      id: styleWidget
      Layout.fillWidth: true
      Layout.preferredHeight: preferredHeight
      clip: true
      discrete: mathObj.discrete
      dataBackend.color: colorButton.selectedColor
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

    Item {
      Layout.preferredHeight: 10
      Layout.fillWidth: true
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
    }
  }


  Component.onCompleted: {
    loader.sync();
    objectTypeTumbler.sync();
    MathWorld.attachStyle(mathObj, styleWidget.dataBackend);
  }

}
