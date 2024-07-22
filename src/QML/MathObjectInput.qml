import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import zegrapher.information 1.0
import zegrapher.zc 1.0

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

  ColumnLayout {
    id: layout
    anchors.fill: parent

    RowLayout {
      Layout.fillWidth: true

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

  EquationEdit {
    id: eqEdit
    type: ZC.AUTO
    Layout.fillWidth: true
  }

  ToolSeparator
  {
    orientation: Qt.Horizontal
    Layout.fillWidth: true
    Layout.topMargin: 10
  }


  }

}
