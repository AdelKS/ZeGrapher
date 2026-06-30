import QtQuick

Item {
  id: root

  required property GraphRange range

  property int spacing: 5
  implicitHeight: yminEdit.y + yminEdit.height

  onImplicitWidthChanged: {
    console.log("RangeAdjust: implicitWidth: ", implicitWidth);
  }

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  Connections {
    target: root.range.y
    function onStateChanged() {
      console.debug("Y range state change: ", root.range.y.state)
      if (root.range.y.state) {
        yminEdit.customErrorMsg = "";
        ymaxEdit.customErrorMsg = "";
      }
      else {
        if (!isNaN(root.range.y.min.value) && !isNaN(root.range.y.max.value) &&
            root.range.y.min.value >= root.range.y.max.value) {
          yminEdit.customErrorMsg = "<b>y</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>y</b><sub>max</sub>";
          ymaxEdit.customErrorMsg = "<b>y</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>y</b><sub>min</sub>";
        }
      }
    }
  }

  Connections {
    target: root.range.x
    function onStateChanged() {
      console.debug("X range state change: ", root.range.x.state)
      if (root.range.x.state) {
        xminEdit.customErrorMsg = "";
        xmaxEdit.customErrorMsg = "";
      }
      else {
        if (!isNaN(root.range.x.min.value) && !isNaN(root.range.x.max.value) &&
            root.range.x.min.value >= root.range.x.max.value) {
          xminEdit.customErrorMsg = "<b>x</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>x</b><sub>max</sub>";
          xmaxEdit.customErrorMsg = "<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>";
        }
      }
    }
  }


  Connections {
    target: root.range
    function onInteractiveUpdate() {
      console.debug("Update x/y min/max widgets after user interaction with graph")
      ymaxEdit.expression = root.range.y.max.expression;
      yminEdit.expression = root.range.y.min.expression;
      xmaxEdit.expression = root.range.x.max.expression;
      xminEdit.expression = root.range.x.min.expression;
    }
  }

  ZeTextEdit {
    id: ymaxLbl
    text: "<b>y</b><sub>max</sub>"
    font: Information.appSettings.font
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

  ValueEdit {
    id: ymaxEdit
    expression: "10"
    anchors.top: ymaxLbl.bottom
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.y.max
  }

  ZeTextEdit {
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

  ValueEdit {
    id: xminEdit
    expression: "-10"
    anchors.left: root.left
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.x.min
  }


  ZeTextEdit {
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

  ValueEdit {
    id: xmaxEdit
    expression: "10"
    anchors.right: root.right
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.x.max
  }

  ZeTextEdit {
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

  ValueEdit {
    id: yminEdit
    expression: "-10"
    y: xminEdit.y + xminEdit.exprHeight + spacing
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.y.min
  }
}
