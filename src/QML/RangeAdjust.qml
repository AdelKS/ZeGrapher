import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import zegrapher.information 1.0

Rectangle {
  id: root

  property int spacing: 5

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  Item {
    id: centerItem
    anchors.centerIn: root
    height: 75
    width: root.width / 3 - 2*root.spacing
  }

  ExpressionEdit {
    id: ymaxEdit
    expression: "10"
    anchors.margins: root.spacing
    anchors.bottom: centerItem.top
    anchors.left: centerItem.left
    anchors.right: centerItem.right

    onValueChanged: {
      if (value > yminEdit.value) {
        Information.range.y.max = value
      } else {
        setCustomErrorMsg("<b>y</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>y</b><sub>min</sub>")
      }
    }
  }

  TextEdit {
    text: "<b>y</b><sub>max</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    anchors.horizontalCenter: ymaxEdit.horizontalCenter
    anchors.bottom: ymaxEdit.top
    anchors.margins: 3
  }

  ExpressionEdit {
    id: xminEdit
    expression: "-10"
    anchors.margins: root.spacing
    anchors.verticalCenter: centerItem.verticalCenter
    anchors.left: root.left
    anchors.right: centerItem.left

    onValueChanged: {
      if (value < xmaxEdit.value) {
        Information.range.x.min = value
      } else {
        setCustomErrorMsg("<b>x</b><sub>min</sub>" + qsTr(" must be less than ") + "<b>x</b><sub>max</sub>")
      }
    }
  }

  TextEdit {
    text: "<b>x</b><sub>min</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    anchors.horizontalCenter: xminEdit.horizontalCenter
    anchors.bottom: xminEdit.top
    anchors.margins: 3
  }

  ExpressionEdit {
    id: xmaxEdit
    expression: "10"
    anchors.margins: root.spacing
    anchors.verticalCenter: centerItem.verticalCenter
    anchors.left: centerItem.right
    anchors.right: root.right

    onValueChanged: {
      if (value > xminEdit.value) {
        Information.range.x.max = value
      } else {
        setCustomErrorMsg("<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>")
      }
    }
  }

  TextEdit {
    text: "<b>x</b><sub>max</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    anchors.horizontalCenter: xmaxEdit.horizontalCenter
    anchors.bottom: xmaxEdit.top
    anchors.margins: 3
  }

  ExpressionEdit {
    id: yminEdit
    expression: "-10"
    anchors.margins: root.spacing
    anchors.top: centerItem.bottom
    anchors.bottom: root.bottom
    anchors.left: centerItem.left
    anchors.right: centerItem.right

    onValueChanged: {
      if (value < ymaxEdit.value) {
        Information.range.y.min = value
      } else {
        setCustomErrorMsg("<b>y</b><sub>min</sub>" + qsTr(" must be less than ") + "<b>y</b><sub>max</sub>")
      }
    }
  }

  TextEdit {
    text: "<b>y</b><sub>min</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    anchors.horizontalCenter: yminEdit.horizontalCenter
    anchors.bottom: yminEdit.top
    anchors.margins: 3
  }

}
