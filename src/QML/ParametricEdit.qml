// The pair of object names that make up a parametric equation

import QtQuick
import QtQuick.Layouts

Item {
  id: root

  required property Parametric backend

  implicitHeight: layout.implicitHeight
  implicitWidth:  layout.implicitWidth + 2 * layout.spacing

  Behavior on height { SmoothedAnimation { duration: 200 } }

  function removeObj() {
    root.opacity = 0;
    root.height = 0;
    root.destroy(200);
  }

  RowLayout {
    id: layout

    anchors.fill: parent
    anchors.leftMargin: spacing
    anchors.rightMargin: spacing

    spacing: 10

    NameEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      backend: root.backend.obj1
      label: qsTr("x object name")
    }

    NameEdit {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignTop

      backend: root.backend.obj2
      label: qsTr("y object name")
    }
  }
}
