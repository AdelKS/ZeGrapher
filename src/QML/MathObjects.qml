import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtCore

Item {
  id: root

  required property size globalMenuSize

  signal importCSV(file: url)

  property real objectsImplicitWidth: 0

  implicitWidth: objectsImplicitWidth + verticalScrollBar.width * verticalScrollBar.visible

  onImplicitWidthChanged: {
    console.debug("MathObjects: implicitWidth: ", implicitWidth)
  }

  ListView {
    id: listView
    anchors.fill: parent
    anchors.margins: 5
    anchors.rightMargin: 0
    clip: true

    model: MathWorld
    spacing: 10

    bottomMargin: root.globalMenuSize.height + 10

    ScrollBar.vertical: ScrollBar {
      id: verticalScrollBar
      policy: listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    }

    move: Transition {
      NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
    }
    moveDisplaced: Transition {
      NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
    }

    delegate: Item {
      id: dragWrapper

      required property int index
      required property var modelData

      width: listView.width - verticalScrollBar.width * verticalScrollBar.visible

      implicitHeight: input.implicitHeight
      height: implicitHeight

      z: dragArea.drag.active ? 2 : 1

      Connections {
        target: dragWrapper.modelData
        function onInitiateUiDelete() {
          collapse.start();
        }
      }

      NumberAnimation {
        id: collapse
        target: dragWrapper
        properties: "height,opacity"
        duration: 300
        to: 0
        easing.type: Easing.InOutQuad

        onFinished: MathWorld.removeMathObject(dragWrapper.modelData)
      }

      MathObjectInput {
        id: input
        width: dragWrapper.width
        height: dragWrapper.height
        mathObj: dragWrapper.modelData

        z: dragArea.drag.active ? 100 : 0

        Component.onCompleted: root.objectsImplicitWidth = Math.max(root.objectsImplicitWidth, implicitWidth)
        onImplicitWidthChanged: root.objectsImplicitWidth = Math.max(root.objectsImplicitWidth, implicitWidth)

        states: [
          State {
            name: "dragging"
            when: dragArea.drag.active
            ParentChange { target: input; parent: listView.contentItem }
          },
          State {
            name: "normal"
            when: !dragArea.drag.active
            ParentChange { target: input; parent: dragWrapper; y: 0 }
          }
        ]

        transitions: Transition {
          from: "dragging"
          to: "normal"
          ParentAnimation {
            NumberAnimation { property: "y"; duration: 400; easing.type: Easing.OutCubic }
          }
        }
      }

      MouseArea {
        id: dragArea
        parent: input.dragHandle
        anchors.fill: parent
        drag.target: input
        drag.axis: Drag.YAxis
        cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor

        onPositionChanged: {
          if (!drag.active)
            return;
          const P = mapToItem(listView.contentItem, mouseX, mouseY);
          const i = dragWrapper.index;
          if (i > 0) {
            const prev = listView.itemAtIndex(i - 1);
            if (prev && P.y < prev.y + prev.height / 2) {
              MathWorld.moveMathObject(i, i - 1);
              return;
            }
          }
          if (i < listView.count - 1) {
            const next = listView.itemAtIndex(i + 1);
            if (next && P.y > next.y + next.height / 2) {
              MathWorld.moveMathObject(i, i + 1);
              return;
            }
          }
        }
      }
    }
  }

  FileDialog {
    id: fileDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
    nameFilters: [qsTr("CSV") + " (*.csv)", qsTr("Text") + " (*.txt)", qsTr("Any") + " (*)"]
    visible: false
    onAccepted: root.importCSV(selectedFile)
  }

  RowLayout
  {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10 + Math.max((root.globalMenuSize.height - height) / 2., 0)
    anchors.margins: 10
    height: implicitHeight
    spacing: 5

    Item {
      Layout.fillWidth: true
      Layout.minimumWidth: root.globalMenuSize.width
    }

    IconRoundButton {
      id: importCSV
      z: 100
      Layout.preferredHeight: 35
      Layout.preferredWidth: 35
      Layout.alignment: Qt.AlignRight

      lightThemeIcon: "qrc:/icons/csv-import-dark.svg"
      darkThemeIcon: "qrc:/icons/csv-import-light.svg"

      onReleased: fileDialog.visible = true;
    }

    IconRoundButton {
      id: add
      z: 100
      Layout.preferredHeight: 35
      Layout.preferredWidth: 35
      Layout.alignment: Qt.AlignRight
      darkThemeIcon: "qrc:/icons/add-light.svg"
      lightThemeIcon: "qrc:/icons/add.svg"

      onReleased: MathWorld.addMathObject();
    }
  }
}
