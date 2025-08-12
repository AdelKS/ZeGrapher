import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

Rectangle {
  id: root

  property alias style: plotStyle

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  Behavior on height { SmoothedAnimation { duration: 200 } }
  Behavior on opacity { SmoothedAnimation { duration: 150 } }

  implicitHeight: layout.implicitHeight
  implicitWidth: firstRow.implicitWidth

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  PlotStyle {
    id: plotStyle
    visible: !displayButton.checked
    color: colorButton.selectedColor
    lineStyle: styleWidget.lineStyle
    lineWidth: styleWidget.lineWidth
    pointStyle: styleWidget.pointStyle
    pointWidth: styleWidget.pointWidth
    coordinateSystem: styleWidget.coordinateSystem
    start: styleWidget.start
    end: styleWidget.end
    step: styleWidget.step
  }

  Column {
    id: layout
    anchors.fill: parent
    spacing: 5

    RowLayout {
      id: firstRow
      width: parent.width
      height: 40

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
            type: ObjectType.EQUATION
            has_graph: true
          }
          ListElement {
            text: "Constant"
            type: ObjectType.CONSTANT
            has_graph: false
          }
          ListElement {
            text: "Parametric"
            type: ObjectType.PARAMETRIC
            has_graph: true
          }
          ListElement {
            text: "Data"
            type: ObjectType.DATA
            has_graph: true
          }
        }

        onCurrentIndexChanged: placeholder.updateObjectType()
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
      width: parent.width
      height: 0
      clip: true
      objectType: plotStyle.objectType

      Behavior on height { SmoothedAnimation { duration: 200 } }

      states: [
        State {
          name: "hidden"; when: !styleButton.checked || !styleButton.visible
          PropertyChanges { styleWidget.height: 0 }
        },
        State {
          name: "shown"; when: styleButton.checked && styleButton.visible
          PropertyChanges {
            explicit: false
            styleWidget.height: styleWidget.implicitHeight
          }
        }
      ]
    }

    Item {
      id: placeholder
      width: parent.width

      property int currentType: 0

      function updateObjectType() {
        if (children.length !== 0 &&
            currentType !== eqTypeModel.get(objectTypeTumbler.currentIndex).type)
        {
          for (var i = 0 ; i != children.length ; i++)
            children[i].removeObj();
        }
        currentType = eqTypeModel.get(objectTypeTumbler.currentIndex).type
        var component;
        if (currentType === ObjectType.EQUATION) {
          component = Qt.createComponent("qrc:/qt/qml/ZeGrapher/EquationEdit.qml");
        } else if (currentType === ObjectType.CONSTANT) {
          component = Qt.createComponent("qrc:/qt/qml/ZeGrapher/ConstantEdit.qml");
        } else if (currentType === ObjectType.PARAMETRIC) {
          component = Qt.createComponent("qrc:/qt/qml/ZeGrapher/ParametricEdit.qml");
        } else if (currentType === ObjectType.DATA) {
          component = Qt.createComponent("qrc:/qt/qml/ZeGrapher/DataEdit.qml");
        }
        else
        {
          console.error("Case not handled");
          return;
        }

        if (component.status !== Component.Ready)
        {
          console.error("Component not ready: ", component.errorString());
          return;
        }

        var widget = component.createObject(placeholder, {"style": root.style})

        if (widget === null) {
          console.debug("Error creating object");
        } else {
          widget.width = Qt.binding(function (){ return placeholder.width });
          placeholder.height = Qt.binding(function (){ return widget.implicitHeight });
        }
      }
    }

    Item {
      height: 10
      width: parent.width
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      width: parent.width
    }
  }

}
