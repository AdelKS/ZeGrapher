import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Item {
  ScrollView {
    id: scrollView
    anchors.fill: parent

    contentWidth: Math.max(availableWidth - ScrollBar.vertical.width * ScrollBar.vertical.visible, mathObjCol.implicitWidth)
    contentHeight: Math.max(availableHeight - ScrollBar.horizontal.height * ScrollBar.horizontal.visible, mathObjCol.implicitHeight)

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

      Item {
        Layout.fillHeight: true
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
