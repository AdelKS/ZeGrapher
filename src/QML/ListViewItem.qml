import QtQuick

Item {
  id: root

  required property ListView view
  required property int index

  // The widget to drag: must be a child of this delegate and expose a dragHandle
  required property Item content
  required property Item dragHandle

  readonly property alias dragging: dragArea.drag.active

  signal moveRequested(int from, int to)
  signal removeRequested()

  implicitHeight: content.implicitHeight

  z: dragging ? 2 : 1

  // the widget follows us, and rides above the other delegates while dragged
  Binding { target: root.content; property: "width"; value: root.width }
  Binding { target: root.content; property: "height"; value: root.height }
  Binding { target: root.content; property: "z"; value: root.dragging ? 100 : 0 }

  function collapseAndRemove() {
    collapse.start();
  }

  NumberAnimation {
    id: collapse
    target: root
    properties: "height,opacity"
    duration: 300
    to: 0
    easing.type: Easing.InOutQuad

    onFinished: root.removeRequested()
  }

  states: [
    State {
      name: "dragging"
      when: root.dragging
      ParentChange { target: root.content; parent: root.view.contentItem }
    },
    State {
      name: "normal"
      when: !root.dragging
      ParentChange { target: root.content; parent: root; y: 0 }
    }
  ]

  transitions: Transition {
    from: "dragging"
    to: "normal"
    ParentAnimation {
      NumberAnimation { property: "y"; duration: 400; easing.type: Easing.OutCubic }
    }
  }

  MouseArea {
    id: dragArea
    parent: root.dragHandle
    anchors.fill: parent
    drag.target: root.content
    drag.axis: Drag.YAxis
    cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor

    // else an enclosing ListView flicks away with the reorder drag
    preventStealing: true

    onPositionChanged: {
      if (!drag.active)
        return;
      const i = root.index;
      const y = mapToItem(root.view.contentItem, mouseX, mouseY).y;
      const prev = i > 0 ? root.view.itemAtIndex(i - 1) : null;
      const next = i < root.view.count - 1 ? root.view.itemAtIndex(i + 1) : null;

      if (prev && y < prev.y + prev.height / 2)
        root.moveRequested(i, i - 1);
      else if (next && y > next.y + next.height / 2)
        root.moveRequested(i, i + 1);
    }
  }
}
