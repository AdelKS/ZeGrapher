// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Parametric backend

  property NamedRef eq1: backend.obj1
  property NamedRef eq2: backend.obj2

  implicitHeight: gridLayout.implicitHeight
  implicitWidth:  gridLayout.implicitWidth

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  GridLayout {
    id: gridLayout

    columns: 5
    anchors.fill: parent

    columnSpacing: 10
    rowSpacing: 0

    Item {}

    ZeLabel {
      Layout.alignment: Qt.AlignHCenter
      id: ref1Label
      text: qsTr("x object name")
    }

    Item {}

    ZeLabel {
      Layout.alignment: Qt.AlignHCenter
      id: ref2Label
      text: qsTr("y object name")
    }

    Item {}

    Item {}

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      expression: root.eq1.name
      onTextEdited: (expression) => root.eq1.setName(expression)

      id: lineEdit1

      state: root.eq1.state
    }

    Item {}

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      expression: root.eq2.name
      onTextEdited: (expression) => root.eq2.setName(expression)

      id: lineEdit2

      state: root.eq2.state
    }

    Item {}
  }
}
