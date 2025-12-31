import QtQuick
import QtQuick.Controls.FluentWinUI3
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
    MathWorld.addMathObject();
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

    TabBar {
      id: bar
      Layout.fillWidth: true

      onCurrentIndexChanged: swipeView.setCurrentIndex(currentIndex)

      TabButton {
        text: qsTr("Math")
      }

      TabButton {
        text: qsTr("App")
      }
    }

    SwipeView {
      id: swipeView

      Layout.fillWidth: true
      Layout.fillHeight: true

      onCurrentIndexChanged: bar.setCurrentIndex(currentIndex)

      MathObjects {}

      AppSettings {}

    }
  }

  Component.onCompleted: {
    addWidget();
  }
}
