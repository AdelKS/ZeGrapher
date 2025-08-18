// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property MathObject mathObj
  required property PlotStyle style

  property Data zcBackend: {
    mathObj.setBackend(MathObject.DATA);
    zcBackend = mathObj.getData();
  }

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  RowLayout {
    anchors.fill: parent

    Label {
      text: qsTr("Name: ")
      Layout.topMargin: 5
      Layout.alignment: Qt.AlignTop | Qt.AlignRight
    }

    ZcExprEdit {
      id: zcExprEdit
      backend: zcBackend

      onExpressionChanged: {
        if (showInTable.checked) {
          console.debug("Updating name in DataTableModel singleton.");
          if (zcBackend.isValid())
            DataTableModel.setColumnName(zcBackend, expression);
          else
            DataTableModel.setColumnName(zcBackend, "");

        }
      }

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter
    }

    RoundButton {
      id: showInTable
      Layout.minimumWidth: 20
      Layout.maximumWidth: 30
      Layout.preferredHeight: Layout.preferredWidth
      Layout.preferredWidth: 30

      checkable: true
      checked: false
      icon.source: "qrc:/icons/table.svg"

      onToggled: {
        if (showInTable.checked) {
          DataTableModel.registerTableColumn(zcBackend);
        } else {
          DataTableModel.deregisterTableColumn(zcBackend);
        }
      }

      display: Button.IconOnly
      padding: 0
    }
  }

  Component.onCompleted: {
    mathObj.style = style;
    console.debug("DataEdit: backend=", zcBackend);
  }

  Component.onDestruction: {
    DataTableModel.deregisterTableColumn(zcBackend);
  }

}
