import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: userInputPanel

  implicitWidth: colLayout.implicitWidth

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    implicitWidth: Math.max(rangeAdjust.implicitWidth, scrollView.implicitWidth)

    RangeAdjust {
      id: rangeAdjust
      Layout.topMargin: 10
      Layout.fillWidth: true
      Layout.bottomMargin: 10
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
      Layout.bottomMargin: 10
    }

    ScrollView {
      id: scrollView
      Layout.fillWidth: true
      Layout.fillHeight: true

      implicitWidth: mathObjCol.minWidth

      contentWidth: availableWidth

      ScrollBar.vertical.policy: ScrollBar.AlwaysOn
      ScrollBar.vertical.interactive: true

      Column {
        id: mathObjCol
        anchors.fill: parent
        spacing: 10

        property int minWidth: 0

        function updateMinWidth(w: int) {
          if (minWidth < w)
            minWidth = w;
        }

        function createMathObjectInput() {
          var component = Qt.createComponent("qrc:/qt/qml/ZeGrapher/MathObjectInput.qml");
          if (component.status === Component.Error) {
            console.error("Failed loading MathObjectInput.qml \n Error message:\n  |", component.errorString().replace("\n", "\n  | "));
            return;
          }

          var inputWidget = component.createObject(mathObjCol)

          if (inputWidget === null) {
            console.error("Error creating object");
            return;
          }

          inputWidget.width = Qt.binding(function (){ return mathObjCol.width - 5 });

          updateMinWidth(inputWidget.implicitWidth);
          inputWidget.implicitWidthChanged.connect(updateMinWidth);
        }

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

    onReleased: mathObjCol.createMathObjectInput()

    icon.width: 2*width/3
    icon.height: 2*width/3
    display: Button.IconOnly
    padding: 0
  }
}
