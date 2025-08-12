// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property PlotStyle style

  implicitHeight: Math.max(lineEdit1.implicitHeight, lineEdit2.implicitHeight)

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  NamedRef {
    id: eq1
  }

  NamedRef {
    id: eq2
  }

  ZcMathObject {
    id: zcObj1
  }

  ZcMathObject {
    id: zcObj2
  }

  Parametric {
    id: parObj
    obj1: zcObj1
    obj2: zcObj2
  }

  MathObject {
    id: mathObj
  }

  RowLayout {
    anchors.fill: parent

    Label {
      Layout.alignment: Qt.AlignTop
      Layout.topMargin: 5
      id: ref1Label
      text: "x: "
      verticalAlignment: Text.AlignTop
    }

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      id: lineEdit1
      mathObj: zcObj1
    }

    Item {
      Layout.preferredWidth: 10
      Layout.preferredHeight: 0
    }

    Label {
      Layout.topMargin: 5
      Layout.alignment: Qt.AlignTop

      id: ref2Label
      text: "y: "
      verticalAlignment: Text.AlignTop
    }

    ZcExprEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      id: lineEdit2
      mathObj: zcObj2
    }
  }

  Component.onCompleted: {
    console.debug("ParametricEdit: backend1=", eq1);
    console.debug("ParametricEdit: backend2=", eq2);
    console.debug("ParametricEdit: style=", style);

    mathObj.style = style;
    zcObj1.setBackend(eq1);
    zcObj2.setBackend(eq2);
    mathObj.setBackend(parObj);
  }
}
