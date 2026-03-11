// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.FluentWinUI3

Item {
  id: root

  property bool pauseSync: false

  required property ZeGraphSettings graphSettings

  implicitWidth: scrollView.implicitWidth

  enum Selection { X_Y, X, Y }

  ScrollView {
    id: scrollView
    anchors.fill: parent
    anchors.margins: 5
    leftPadding: Math.max((width - mainLayout.implicitWidth)/2, 0)
    implicitWidth: mainLayout.implicitWidth + ScrollBar.vertical.width + anchors.margins * 2

    ColumnLayout {
      id: mainLayout
      anchors.fill: parent
    }
  }
}
