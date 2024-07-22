import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import zegrapher.information 1.0

Item {
  id: userInputPanel

  implicitWidth: colLayout.implicitWidth

  function createMathObjectInput() {
    var component = Qt.createComponent("qrc:/src/QML/MathObjectInput.qml");
    var inputWidget = component.createObject(mathObjCol)

    inputWidget.width = Qt.binding(function (){ return mathObjCol.width - 5 });

    if (inputWidget === null) {
      console.log("Error creating object");
    }
  }

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    RangeAdjust {
      Layout.fillWidth: true
      Layout.bottomMargin: 20
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
      Layout.bottomMargin: 20
    }

    ScrollView {
      id: scrollView
      Layout.fillWidth: true
      Layout.fillHeight: true

      contentWidth: availableWidth

      ScrollBar.vertical.policy: ScrollBar.AlwaysOn
      ScrollBar.vertical.interactive: true

      Column {
        id: mathObjCol
        anchors.fill: parent
        spacing: 10

        onImplicitHeightChanged: console.log("layout implicit height: ", implicitHeight)

        Component.onCompleted: createMathObjectInput()
      }

    }
  }

  RoundButton {
    id: add
    z: 100
    width: 35
    height: width

    x: userInputPanel.width - width
    y: userInputPanel.height - width - 10

    icon.source: "qrc:/icons/add.svg"

    onReleased: createMathObjectInput()

    icon.width: 2*width/3
    icon.height: 2*width/3
    display: Button.IconOnly
    padding: 0
  }
}
