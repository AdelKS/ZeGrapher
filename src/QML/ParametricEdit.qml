// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property MathObject mathObj
  required property PlotStyle style

  property Parametric parametric: {
    mathObj.setBackend(MathObject.PARAMETRIC);
    parametric = mathObj.getParametric();
  }

  readonly property ZcMathObject zcObj1: parametric.getObj1()
  readonly property NamedRef eq1: zcObj1.getNamedRef();

  readonly property ZcMathObject zcObj2: parametric.getObj2()
  readonly property NamedRef eq2: zcObj2.getNamedRef();

  implicitHeight: Math.max(lineEdit1.implicitHeight, lineEdit2.implicitHeight)

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
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
      backend: eq1
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
      backend: eq2
    }
  }

  Component.onCompleted: {
    console.debug("ParametricEdit: backend1=", eq1);
    console.debug("ParametricEdit: backend2=", eq2);
    console.debug("ParametricEdit: style=", style);
    mathObj.style = style;
  }
}
