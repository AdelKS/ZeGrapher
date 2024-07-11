import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import zegrapher.information 1.0
import zegrapher.expreditbackend 1.0

Rectangle {

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  implicitHeight: layout.implicitHeight
  implicitWidth: layout.implicitWidth

  ColumnLayout {
    id: layout
    anchors.fill: parent

    RowLayout {
      Layout.fillWidth: true

      ComboBox {
        Layout.fillWidth: false
        Layout.maximumWidth: implicitContentWidth + 20
        model: ["Auto", "Function", "Sequence", "Constant"]
        implicitContentWidthPolicy: ComboBox.WidestText
        onCurrentTextChanged: {
          switch (currentText)
          {
          case "Auto":
            exprEdit.type = ExprEditBackend.OBJECT_AUTO;
            break;
          case "Function":
            exprEdit.type = ExprEditBackend.OBJECT_FUNCTION;
            break;
          case "Sequence":
            exprEdit.type = ExprEditBackend.OBJECT_SEQUENCE;
            break;
          case "Constant":
            exprEdit.type = ExprEditBackend.OBJECT_GLOBAL_CONSTANT;
            break;
          default:
            console.error("Error in switch statement");
          }
        }
      }

      Item {
        Layout.fillWidth: true
      }

      ColorButton {
        Layout.fillHeight: true
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30
      }
    }

    ExpressionEdit {
      id: exprEdit
      Layout.fillWidth: true
    }

  }

}
