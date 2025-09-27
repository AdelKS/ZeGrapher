// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  id: root

  required property Data backend

  property alias name: zcExprEdit.expression
  property alias exprEdit: zcExprEdit
  readonly property alias exprHeight: zcExprEdit.exprHeight

  implicitHeight: zcExprEdit.implicitHeight

  Behavior on height { SmoothedAnimation { duration: 200 } }

  Connections {
    target: backend
    function onNameChanged() {
      if (root.name != backend.name)
        root.name = backend.name;
    }
  }

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

      Connections {
        target: root.backend
        function onStateChanged() {
          zcExprEdit.setState(root.backend.state);
        }
      }

      onTextEdited: {
        console.debug("DataEdit: updating name in backend: ", expression);
        backend.setName(expression);
        if (showInTable.checked) {
          console.debug("DataEdit: updating name in DataTableModel singleton.");
          if (backend.isValid())
            DataTableModel.setColumnName(backend, expression);
          else
            DataTableModel.setColumnName(backend, "");

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
          DataTableModel.registerTableColumn(backend);
        } else {
          DataTableModel.deregisterTableColumn(backend);
        }
      }

      display: Button.IconOnly
      padding: 0
    }
  }

  Component.onCompleted: {
    console.debug("DataEdit: backend=", backend);
  }

  Component.onDestruction: {
    DataTableModel.deregisterTableColumn(backend);
  }

}
