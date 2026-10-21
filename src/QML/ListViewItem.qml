/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

import QtQuick

Item {
  id: root

  required property int index
  required property var modelData

  readonly property ReorderList view: ListView.view as ReorderList

  // The widget to drag: must be a child of this delegate and expose a dragHandle
  required property Item content
  required property Item dragHandle

  readonly property alias dragging: dragArea.drag.active

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

    onFinished: root.view.removeRequested(root.modelData)
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
        root.view.moveRequested(i, i - 1);
      else if (next && y > next.y + next.height / 2)
        root.view.moveRequested(i, i + 1);
    }
  }
}
