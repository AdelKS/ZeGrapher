import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: userInputPanel
  clip: true

  required property ZeGraphSettings graphSettings

  implicitWidth: colLayout.implicitWidth

  onImplicitWidthChanged: {
    console.debug("UserInputPanel: implicitWidth: ", implicitWidth);
  }

  onImplicitHeightChanged: {
    console.debug("UserInputPanel: implicitHeight: ", implicitHeight);
  }

  ColumnLayout {
    id: colLayout
    anchors.fill: parent

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

    TabBar {
      id: bar
      Layout.fillWidth: true

      onCurrentIndexChanged: swipeView.setCurrentIndex(currentIndex)

      implicitWidth: mathTabButton.implicitWidth + graphTabButton.implicitWidth + appTabButton.implicitWidth

      onImplicitWidthChanged: {
        console.log("UserInputPanel: TabBar: implicitWidth: ", implicitWidth);
      }

      ZeTabButton {
        id: mathTabButton
        text: qsTr("Math")
      }

      ZeTabButton {
        id: gridTabButton
        text: qsTr("Grid")
      }

      ZeTabButton {
        id: graphTabButton
        text: qsTr("Graph")
      }

      ZeTabButton {
        id: appTabButton
        text: qsTr("App")
      }
    }

    SwipeView {
      id: swipeView

      Layout.fillWidth: true
      Layout.fillHeight: true

      implicitWidth: Math.max(mathObjects.implicitWidth, gridSettings.implicitWidth, graphSettings.implicitWidth, appSettings.implicitWidth)

      onCurrentIndexChanged: bar.setCurrentIndex(currentIndex)

      onImplicitWidthChanged: {
        console.log("UserInputPanel: SwipeView: implicitWidth: ", implicitWidth);
      }

      MathObjects {
        id: mathObjects
      }

      GridSettings {
        id: gridSettings
        graphSettings: userInputPanel.graphSettings
      }

      GraphSettings {
        id: graphSettings
        graphSettings: userInputPanel.graphSettings
      }

      AppSettings {
        id: appSettings
      }

    }
  }
}
