import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import zegrapher.information 1.0
import zegrapher.zc 1.0
import zegrapher.mathobject 1.0

Rectangle {
  id: root

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  Behavior on height { SmoothedAnimation { duration: 200 } }
  Behavior on opacity { SmoothedAnimation { duration: 150 } }

  implicitHeight: layout.implicitHeight
  implicitWidth: layout.implicitWidth

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  MathObject {
    id: mathObj
  }

  Column {
    id: layout
    anchors.fill: parent
    spacing: 5

    RowLayout {
      id: firstRow
      width: parent.width
      height: implicitHeight+4

      ComboBox {
        Layout.fillWidth: false
        Layout.maximumWidth: implicitContentWidth + 30
        model: ["Auto", "Function", "Sequence", "Constant"]
        implicitContentWidthPolicy: ComboBox.WidestText
        onCurrentTextChanged: {
          switch (currentText)
          {
          case "Auto":
            eqEdit.type = ZC.AUTO;
            break;
          case "Function":
            eqEdit.type = ZC.FUNCTION;
            break;
          case "Sequence":
            eqEdit.type = ZC.SEQUENCE;
            break;
          case "Constant":
            eqEdit.type = ZC.CONSTANT;
            break;
          default:
            console.error("Error in switch statement");
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

        checkable: true
        checked: false

        icon.source: checked ? "qrc:/icons/closed-eye.svg" : "qrc:/icons/open-eye.svg"

        icon.width: 2*width/3
        icon.height: 2*width/3
        display: Button.IconOnly
        padding: 0
      }

      RoundButton {
        id: styleButton
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30

        checkable: true
        checked: false

        icon.source: "qrc:/icons/brush.svg"

        icon.width: 2*width/3
        icon.height: 2*width/3
        display: Button.IconOnly
        padding: 0
      }

      ColorButton {
        radius: 12
      }

      Item {
        Layout.fillWidth: true
      }

      RoundButton {
        id: del
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30

        icon.source: "qrc:/icons/delete.svg"

        onReleased: removeObj()

        icon.width: 2*width/3
        icon.height: 2*width/3
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
      type: ZC.AUTO
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
