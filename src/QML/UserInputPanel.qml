import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
  id: userInputPanel
  clip: true

  implicitWidth: colLayout.implicitWidth

  onImplicitWidthChanged: {
    console.debug("UserInputPanel: implicitWidth: ", implicitWidth);
  }

  onImplicitHeightChanged: {
    console.debug("UserInputPanel: implicitHeight: ", implicitHeight);
  }

  onHeightChanged: {
    console.debug("UserInputPanel: height: ", height);
  }

  onWidthChanged: {
    console.debug("UserInputPanel: width: ", width);
  }

  function addWidget() {
    mathWidgetList.append({});
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
      contentHeight: mathObjCol.implicitHeight

      implicitWidth: mathObjCol.implicitWidth + ScrollBar.vertical.width
      implicitHeight: mathObjCol.implicitHeight + ScrollBar.horizontal.height

      onContentHeightChanged: {
        console.debug("UserInputPanel: ScrollView: contentHeight=", contentHeight);
      }

      onHeightChanged: {
        console.debug("UserInputPanel: ScrollView: height=", height);
      }

      ScrollBar.vertical.policy: ScrollBar.AlwaysOff

      onAvailableWidthChanged: {
        console.debug("UserInputPanel: ScrollView, availableWidth: ", availableWidth);
      }

      onImplicitWidthChanged: {
        console.debug("UserInputPanel: ScrollView: implicitWidth: ", implicitWidth)
      }

      ColumnLayout {
        id: mathObjCol
        spacing: 10
        anchors.fill: parent

        onImplicitWidthChanged: {
          console.debug("UserInputPanel: ColumnLayout: implicitWidth", implicitWidth);
        }

        onImplicitHeightChanged: {
          console.debug("UserInputPanel: ColumnLayout: implicitHeight", implicitHeight);
        }

        Repeater {
          model: mathWidgetList
          delegate: MathObjectInput {
            required property int index
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop

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

  Component.onCompleted: {
    scrollView.ScrollBar.vertical.policy = Qt.binding(function() {
      return scrollView.contentHeight > scrollView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
    });
  }
}
