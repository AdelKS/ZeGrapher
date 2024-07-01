import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import zegrapher.information 1.0

Rectangle {
  id: root

  property int spacing: 5
  property bool disableSignals: false
  implicitHeight: yminEdit.y + yminEdit.height

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  Connections {
    target: Information
    function onGraphRangeChanged() {
      disableSignals = true;
      ymaxEdit.expression = Number(Information.range.y.max).toLocaleString();
      yminEdit.expression = Number(Information.range.y.min).toLocaleString();
      xmaxEdit.expression = Number(Information.range.x.max).toLocaleString();
      xminEdit.expression = Number(Information.range.x.min).toLocaleString();
      disableSignals = false;
    }
    enabled: !disableSignals
  }


  TextEdit {
    id: ymaxLbl
    text: "<b>y</b><sub>max</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    horizontalAlignment: TextEdit.AlignHCenter
    verticalAlignment: TextEdit.AlignBottom
    color: myPalette.text
    anchors.top: root.top
    anchors.horizontalCenter: root.horizontalCenter
    width: parent.width/3
  }

  ExpressionEdit {
    id: ymaxEdit
    expression: "10"
    anchors.top: ymaxLbl.bottom
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        if (ymaxEdit.value > yminEdit.value) {
          var oldVal = disableSignals;
          disableSignals = true;
          Information.range.y.max = ymaxEdit.value;
          disableSignals = oldVal;
        } else {
          setCustomErrorMsg("<b>y</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>y</b><sub>min</sub>")
        }
      }
      enabled: !disableSignals
    }
  }


  TextEdit {
    text: "<b>x</b><sub>min</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    horizontalAlignment: TextEdit.AlignHCenter
    verticalAlignment: TextEdit.AlignBottom
    anchors.horizontalCenter: xminEdit.horizontalCenter
    anchors.bottomMargin: 5
    anchors.bottom: xminEdit.top
  }

  ExpressionEdit {
    id: xminEdit
    expression: "-10"
    anchors.left: root.left
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        if (xminEdit.value < xmaxEdit.value) {
          var oldVal = disableSignals;
          disableSignals = true;
          Information.range.x.min = xminEdit.value
          disableSignals = oldVal;
        } else {
          setCustomErrorMsg("<b>x</b><sub>min</sub>" + qsTr(" must be less than ") + "<b>x</b><sub>max</sub>")
        }
      }
      enabled: !disableSignals
    }
  }


  TextEdit {
    text: "<b>x</b><sub>max</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    horizontalAlignment: TextEdit.AlignHCenter
    verticalAlignment: TextEdit.AlignBottom
    anchors.horizontalCenter: xmaxEdit.horizontalCenter
    anchors.bottomMargin: 5
    anchors.bottom: xmaxEdit.top
  }

  ExpressionEdit {
    id: xmaxEdit
    expression: "10"
    anchors.right: root.right
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        if (xmaxEdit.value > xminEdit.value) {
          var oldVal = disableSignals;
          disableSignals = true;
          Information.range.x.max = xmaxEdit.value
          disableSignals = oldVal;
        } else {
          setCustomErrorMsg("<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>")
        }
      }
      enabled: !disableSignals
    }
  }

  TextEdit {
    text: "<b>y</b><sub>min</sub>"
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    color: myPalette.text
    horizontalAlignment: TextEdit.AlignHCenter
    verticalAlignment: TextEdit.AlignBottom
    anchors.bottom: yminEdit.top
    anchors.bottomMargin: 5
    anchors.horizontalCenter: root.horizontalCenter

  }

  ExpressionEdit {
    id: yminEdit
    expression: "-10"
    y: xminEdit.y + xminEdit.exprHeight + spacing
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        if (yminEdit.value < ymaxEdit.value) {
          var oldVal = disableSignals;
          disableSignals = true;
          Information.range.y.min = yminEdit.value
          disableSignals = oldVal;
        } else {
          setCustomErrorMsg("<b>y</b><sub>min</sub>" + qsTr(" must be less than ") + "<b>y</b><sub>max</sub>")
        }
      }
      enabled: !disableSignals
    }
  }
}
