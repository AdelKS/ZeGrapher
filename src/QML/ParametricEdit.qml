// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property PlotStyle style

  implicitHeight: lineEdit1.implicitHeight

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
      id: ref1Label
      text: "x: "
    }

    ZcExprEdit {
      Layout.fillWidth: true

      id: lineEdit1
      mathObj: zcObj1
    }

    Item {
      Layout.preferredWidth: 10
      Layout.preferredHeight: 0
    }

    Label {
      id: ref2Label
      text: "y: "
    }

    ZcExprEdit {
      Layout.fillWidth: true

      id: lineEdit2
      mathObj: zcObj2
    }
  }

  Component.onCompleted: {
    console.log("ParametricEdit: backend1=", eq1);
    console.log("ParametricEdit: backend2=", eq2);
    console.log("ParametricEdit: style=", style);

    mathObj.style = style;
    zcObj1.setBackend(eq1);
    zcObj2.setBackend(eq2);
    mathObj.setBackend(parObj);
  }
}
