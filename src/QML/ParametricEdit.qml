// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property MathObject mathObj
  required property PlotStyle style

  property Parametric parametric

  property ZcMathObject zcObj1
  property NamedRef eq1

  property ZcMathObject zcObj2
  property NamedRef eq2

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
    mathObj.setBackend(MathObject.PARAMETRIC);
    parametric = mathObj.getParametric();
    zcObj1 = parametric.getObj1();
    zcObj2 = parametric.getObj2();
    eq1 = zcObj1.getNamedRef();
    eq2 = zcObj1.getNamedRef();
    console.debug("ParametricEdit: backend1=", eq1);
    console.debug("ParametricEdit: backend2=", eq2);
    console.debug("ParametricEdit: style=", style);
    mathObj.style = style;
  }
}
