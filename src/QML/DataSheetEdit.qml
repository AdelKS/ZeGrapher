// The columns of a DataSheet, listed as editable rows

import QtQuick
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  required property DataSheet sheet

  /// @brief how tall the box grows on its own before the list starts scrolling
  property real maxAutoHeight: 300

  /// @brief height the user dragged the box to, zero as long as they haven't
  property real userHeight: 0

  /// @brief the bottom strip that holds the add button and the resize handle
  readonly property real minimumHeight: listView.bottomMargin + resizeHandle.height

  property real columnsImplicitWidth: 0

  readonly property real scrollBarWidth: scrollView.ScrollBar.vertical.visible
    ? scrollView.ScrollBar.vertical.width
    : 0

  readonly property real autoHeight: listView.contentHeight + minimumHeight

  implicitWidth: columnsImplicitWidth + scrollBarWidth
  implicitHeight: Math.max(minimumHeight, userHeight > 0 ? userHeight : Math.min(autoHeight, maxAutoHeight))

  onImplicitWidthChanged: {
    console.debug("DataSheetEdit: implicitWidth: ", implicitWidth)
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.bottomMargin: resizeHandle.height
    clip: true

    ScrollBar.vertical.policy: listView.contentHeight > listView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

    // a vertical ListView leaves contentWidth at -1, which the style would
    // render as a stray collapsed handle
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    ListView {
      id: listView

      model: root.sheet
      spacing: 5

      bottomMargin: addColumn.height + 15

      move: Transition {
        NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
      }
      moveDisplaced: Transition {
        NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutCubic }
      }

      delegate: ListViewItem {
        id: dragWrapper

        required property var modelData

        view: listView
        content: input
        dragHandle: input.dragHandle

        width: listView.width - root.scrollBarWidth

        onMoveRequested: (from, to) => root.sheet.moveColumn(from, to)
        onRemoveRequested: root.sheet.deleteColumn(dragWrapper.modelData)

        DataColumnEdit {
          id: input
          backend: dragWrapper.modelData

          onDeleteRequested: dragWrapper.collapseAndRemove()

          Component.onCompleted: root.columnsImplicitWidth = Math.max(root.columnsImplicitWidth, implicitWidth)
          onImplicitWidthChanged: root.columnsImplicitWidth = Math.max(root.columnsImplicitWidth, implicitWidth)
        }
      }
    }
  }

  IconRoundButton {
    id: addColumn
    z: 100
    width: 30
    height: 30
    anchors.right: parent.right
    anchors.bottom: resizeHandle.top
    anchors.margins: 5

    darkThemeIcon: "qrc:/icons/add-light.svg"
    lightThemeIcon: "qrc:/icons/add.svg"

    onReleased: root.sheet.addColumn();
  }

  MouseArea {
    id: resizeHandle
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: 10
    z: 100

    hoverEnabled: true
    cursorShape: Qt.SizeVerCursor

    // else the ListView we sit in flicks away with our drag
    preventStealing: true

    property real pressY: 0

    onPressed: (mouse) => pressY = mouse.y

    // we follow the box we resize, so the offset the mouse gained on us is the growth asked for
    onPositionChanged: (mouse) => {
      if (pressed)
        root.userHeight = Math.max(root.minimumHeight, root.height + mouse.y - pressY);
    }

    // back to growing with the content
    onDoubleClicked: root.userHeight = 0

    Rectangle {
      anchors.centerIn: parent
      width: 40
      height: 3
      radius: height / 2
      color: resizeHandle.pressed || resizeHandle.containsMouse ? root.palette.highlight : root.palette.mid
    }
  }
}
