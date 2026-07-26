// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property Data backend

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: gridLayout.implicitHeight
  implicitWidth:  gridLayout.implicitWidth

  Behavior on height { SmoothedAnimation { duration: 200 } }

  Connections {
    target: root.backend
    function onNameChanged() {
      if (root.name != root.backend.name)
        root.name = root.backend.name;
    }
  }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  GridLayout {
    id: gridLayout

    columns: 2
    anchors.fill: parent

    ZeLabel {
      text: qsTr("name")
      Layout.alignment: Qt.AlignHCenter
    }

    Item {}

    ZcExprEdit {
      id: zcExprEdit

      state: root.backend.state

      expression: root.backend.name

      onTextEdited: {
        console.debug("DataEdit: updating name in backend: ", expression);
        root.backend.setName(expression);
        if (showInTable.checked) {
          console.debug("DataEdit: updating name in DataTableModel singleton.");
          if (backend.isValid())
            DataTableModel.setColumnName(root.backend, expression);
          else
            DataTableModel.setColumnName(root.backend, "");

        }
      }

      Layout.fillWidth: true
      Layout.alignment: Qt.AlignVCenter
    }

    IconRoundButton {
      id: showInTable
      Layout.minimumWidth: 20
      Layout.maximumWidth: 30
      Layout.preferredHeight: Layout.preferredWidth
      Layout.preferredWidth: 30

      checkable: true
      checked: root.backend.showInTable
      lightThemeIcon: "qrc:/icons/table.svg"
      darkThemeIcon: "qrc:/icons/table-light.svg"

      onToggled: root.backend.showInTable = checked;
    }
  }

  Component.onCompleted: {
    console.debug("DataEdit: backend=", root.backend);
  }

}
