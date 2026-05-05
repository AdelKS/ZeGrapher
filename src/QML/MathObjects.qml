import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {

  onImplicitWidthChanged: {
    console.debug("MathObjects: implicitWidth: ", implicitWidth)
  }

  implicitWidth: scrollView.implicitWidth

  ScrollView {
    id: scrollView
    anchors.fill: parent

    contentWidth: Math.max(availableWidth - ScrollBar.vertical.width * ScrollBar.vertical.visible, mathObjCol.implicitWidth)
    contentHeight: Math.max(availableHeight - ScrollBar.horizontal.height * ScrollBar.horizontal.visible, mathObjCol.implicitHeight)

    implicitWidth: mathObjCol.implicitWidth + ScrollBar.vertical.width
    implicitHeight: mathObjCol.implicitHeight + ScrollBar.horizontal.height

    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mathObjCol
      spacing: 10
      anchors.fill: parent
      anchors.margins: 5

      move: Transition {
        NumberAnimation {
          properties: "y";
          duration: 400;
          easing.type: Easing.OutCubic
        }
      }

      Repeater {
        id: rep
        model: MathWorld
        delegate: Item {
          id: wrapper

          required property int index
          required property var modelData

          property bool justDropped: false
          property int lastSwappedWithY: -1

          width: Math.max(mathObjCol.width, input.implicitWidth)
          height: input.implicitHeight

          z: dragArea.drag.active ? 100 : 0

          MathObjectInput {
            id: input
            width: wrapper.width
            height: wrapper.height
            mathObj: wrapper.modelData.mathObj
            style: wrapper.modelData.style

            states: [
              State {
                name: "dragging"
                when: dragArea.drag.active
                ParentChange { target: input; parent: scrollView.contentItem }
              },
              State {
                name: "normal"
                when: !dragArea.drag.active
                ParentChange { target: input; parent: wrapper; y: 0; }
              }
            ]

            transitions: Transition {
              from: "dragging"
              to: "normal"
              ParentAnimation {
                NumberAnimation {
                  property: "y"
                  duration: 400
                  easing.type: Easing.OutCubic
                }
              }
            }

            onDeleteMe: {
              MathWorld.removeMathObject(wrapper.modelData.mathObj);
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
              const P = mapToItem(mathObjCol, mouseX, mouseY);
              const i = wrapper.index;
              if (i > 0) {
                const prev = rep.itemAt(i - 1);
                if (prev && P.y < prev.y + prev.height / 2) {
                  MathWorld.moveMathObject(i, i - 1);
                  return;
                }
              }
              if (i < rep.count - 1) {
                const next = rep.itemAt(i + 1);
                if (next && P.y > next.y + next.height / 2) {
                  MathWorld.moveMathObject(i, i + 1);
                  return;
                }
              }
            }
          }
        }
      }

    }

    Component.onCompleted: {
      scrollView.ScrollBar.vertical.policy = Qt.binding(function() {
        return scrollView.contentHeight > scrollView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
      });
    }
  }

  RowLayout
  {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.margins: 10
    height: implicitHeight
    spacing: 5

    Item {
      Layout.fillWidth: true
    }

    IconRoundButton {
      id: importCSV
      z: 100
      Layout.preferredHeight: 35
      Layout.preferredWidth: 35
      Layout.alignment: Qt.AlignRight

      lightThemeIcon: "qrc:/icons/csv-import.svg"
      darkThemeIcon: "qrc:/icons/csv-import-light.svg"

      onReleased: fileDialog.visible = true
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

  Component.onCompleted: {
    MathWorld.addMathObject();
  }
}
