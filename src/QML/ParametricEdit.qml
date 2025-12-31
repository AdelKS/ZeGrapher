// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Parametric backend

  property NamedRef eq1: backend.obj1
  property NamedRef eq2: backend.obj2

  implicitHeight: Math.max(lineEdit1.implicitHeight, lineEdit2.implicitHeight)

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  RowLayout {
    anchors.fill: parent

    ZeLabel {
      Layout.alignment: Qt.AlignTop
      Layout.topMargin: 5
      id: ref1Label
      text: "x: "
      verticalAlignment: Text.AlignTop
    }

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      onExpressionChanged: root.eq1.setName(expression);

      id: lineEdit1

      state: root.eq1.state
    }

    Item {
      Layout.preferredWidth: 10
      Layout.preferredHeight: 0
    }

    ZeLabel {
      Layout.topMargin: 5
      Layout.alignment: Qt.AlignTop

      id: ref2Label
      text: "y: "
      verticalAlignment: Text.AlignTop
    }

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      onExpressionChanged: root.eq2.setName(expression);

      id: lineEdit2

      state: root.eq2.state
    }
  }

  Component.onCompleted: {
    console.debug("backendEdit: backend1=", eq1);
    console.debug("backendEdit: backend2=", eq2);
  }
}
