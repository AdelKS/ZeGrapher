import QtQuick

Rectangle {
  id: root

  property int spacing: 5
  implicitHeight: yminEdit.y + yminEdit.height

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  function checkHorizontalWindow() {
    console.debug("checking x range: ", Information.range.x.min, " to ", Information.range.x.max);
    if (Information.range.x.min >= Information.range.x.max) {
      console.debug('setting x range as invalid');
      xminEdit.customErrorMsg = "<b>x</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>x</b><sub>max</sub>";
      xmaxEdit.customErrorMsg = "<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>";
    } else {
      xminEdit.customErrorMsg = "";
      xmaxEdit.customErrorMsg = "";
    }
  }

  function checkVerticalWindow() {
    console.debug("checking y range: ", Information.range.y.min, " to ", Information.range.y.max);
    if (Information.range.y.min >= Information.range.y.max) {
      console.debug('setting y range as invalid');
      yminEdit.customErrorMsg = "<b>y</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>y</b><sub>max</sub>";
      ymaxEdit.customErrorMsg = "<b>y</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>y</b><sub>min</sub>";
    } else {
      yminEdit.customErrorMsg = "";
      ymaxEdit.customErrorMsg = "";
    }
  }

  Connections {
    target: Information
    function onRangeChangedWithMouse() {
      console.debug("Update x/y min/max widgets after user interaction with graph")
      ymaxEdit.expression = Information.range.y.max.expression;
      yminEdit.expression = Information.range.y.min.expression;
      xmaxEdit.expression = Information.range.x.max.expression;
      xminEdit.expression = Information.range.x.min.expression;
    }
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

  ValueEdit {
    id: ymaxEdit
    expression: "10"
    anchors.top: ymaxLbl.bottom
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins
    backend: Information.range.y.max

    Connections {
      target: Information.range.y.max
      function onValueChanged() {
        root.checkVerticalWindow();
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
    backend: Information.range.x.min

    Connections {
      target: Information.range.x.min
      function onValueChanged() {
        root.checkHorizontalWindow();
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
    backend: Information.range.x.max

    Connections {
      target: Information.range.x.max
      function onValueChanged() {
        root.checkHorizontalWindow();
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
    backend: Information.range.y.min

    Connections {
      target: Information.range.y.min
      function onValueChanged() {
        root.checkVerticalWindow();
      }
    }
  }
}
