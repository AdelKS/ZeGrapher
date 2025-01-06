import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

Rectangle {
  id: root

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
  }

  MathObject {
    id: mathObj
    style: plotStyle
  }

  Column {
    id: layout
    anchors.fill: parent
    spacing: 5

    RowLayout {
      id: firstRow
      width: parent.width
      height: 40

      TextTumbler {
        id: objectTypeTumbler
        Layout.fillWidth: false
        Layout.maximumWidth: implicitWidth + 30
        model: ListModel {
          id: eqTypeModel
          ListElement {
            txt: "Equation"
            type: ObjectType.EQUATION
          }
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
    }

    ObjectStyle {
      id: styleWidget
      width: parent.width
      height: 0
      clip: true
      showPointSettings: false

      Behavior on height { SmoothedAnimation { duration: 200 } }

      states: [
        State {
          name: "hidden"; when: !styleButton.checked
          PropertyChanges { styleWidget.height: 0 }
        },
        State {
          name: "shown"; when: styleButton.checked
          PropertyChanges { styleWidget.height: 80 }
        }
      ]
    }

    EquationEdit {
      id: eqEdit
      width: parent.width

      Component.onCompleted: mathObj.setBackend(eqEdit.backend)
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
