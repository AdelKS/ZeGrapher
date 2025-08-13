import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: userInputPanel
  clip: true

  implicitWidth: colLayout.implicitWidth

  onImplicitWidthChanged: {
    console.debug("UserInputPanel: new implicit width: ", implicitWidth);
  }

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

    implicitWidth: Math.max(rangeAdjust.implicitWidth, scrollView.implicitContentWidth)

    RangeAdjust {
      id: rangeAdjust
      Layout.topMargin: 10
      Layout.fillWidth: true
      Layout.bottomMargin: 10
    }

    ToolSeparator
    {
      orientation: Qt.Horizontal
      Layout.fillWidth: true
      Layout.bottomMargin: 10
    }

    ScrollView {
      id: scrollView
      Layout.fillWidth: true
      Layout.fillHeight: true

      contentWidth: Math.max(availableWidth, mathObjCol.implicitWidth)

      implicitWidth: mathObjCol.implicitWidth + ScrollBar.vertical.width

      onAvailableWidthChanged: {
        console.debug("UserInputPanel: scroll view, available width: ", availableWidth);
      }

      onImplicitWidthChanged: {
        console.debug("UserInputPanel: scroll view: implicit width: ", implicitWidth)
      }

      ColumnLayout {
        id: mathObjCol
        spacing: 10
        anchors.fill: parent

        onImplicitWidthChanged: {
          console.debug("UserInputPanel: column layout: new implicit width", implicitWidth);
        }

        Repeater {
          model: mathWidgetList
          delegate: MathObjectInput {
            required property int index
            Layout.fillWidth: true

            onDeleteMe: {
              mathWidgetList.remove(index);
            }
          }
        }
      }


    }
  }

  ListModel {
    id: mathWidgetList
    ListElement {}
  }

  RoundButton {
    id: add
    z: 100
    width: 35
    height: width

    x: userInputPanel.width - width
    y: userInputPanel.height - width - 10

    icon.source: "qrc:/icons/add.svg"

    onReleased: mathWidgetList.append({})

    icon.width: 2*width/3
    icon.height: 2*width/3
    display: Button.IconOnly
    padding: 0
  }
}
