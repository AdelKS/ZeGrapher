import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

ScrollView {
  id: scrollView

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
    anchors.margins: 5

    onImplicitWidthChanged: {
      console.debug("UserInputPanel: ColumnLayout: implicitWidth", implicitWidth);
    }

    onImplicitHeightChanged: {
      console.debug("UserInputPanel: ColumnLayout: implicitHeight", implicitHeight);
    }

    Repeater {
      model: MathWorld
      delegate: MathObjectInput {
        required property MathObject modelData

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        mathObj: modelData

        onDeleteMe: {
          MathWorld.removeMathObject(modelData);
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
