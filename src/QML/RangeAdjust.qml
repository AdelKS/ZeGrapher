import QtQuick

Rectangle {
  id: root

  property int spacing: 5
  property bool disableSignals: false
  implicitHeight: yminEdit.y + yminEdit.height

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  color: myPalette.window

  GraphRange {
    id: graphRange
    x.min: xminEdit.backend
    x.max: xmaxEdit.backend
    y.min: yminEdit.backend
    y.max: ymaxEdit.backend
  }

  function checkHorizontalWindow() {
    console.debug("checking x range: ", xminEdit.value, " to ", xmaxEdit.value);
    var oldVal = disableSignals;
    root.disableSignals = true;
    if (xminEdit.value < xmaxEdit.value) {
      console.debug("Updating information singleton with new x range.");
      xminEdit.customErrorMsg = "";
      xmaxEdit.customErrorMsg = "";
    } else {
      console.debug('setting x range as invalid');
      xminEdit.customErrorMsg = "<b>x</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>x</b><sub>max</sub>";
      xmaxEdit.customErrorMsg = "<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>";
    }
    disableSignals = oldVal;
  }

  function checkVerticalWindow() {
    console.debug("checking y range: ", yminEdit.value, " to ", ymaxEdit.value);
    var oldVal = disableSignals;
    root.disableSignals = true;
    if (yminEdit.value < ymaxEdit.value) {
      console.debug("Updating information singleton with new y range.");
      yminEdit.customErrorMsg = "";
      ymaxEdit.customErrorMsg = "";
    } else {
      console.debug('setting y range as invalid');
      yminEdit.customErrorMsg = "<b>x</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>x</b><sub>max</sub>";
      ymaxEdit.customErrorMsg = "<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>";
    }
    disableSignals = oldVal;
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
    implicitName: "ymax"
    expression: "10"
    anchors.top: ymaxLbl.bottom
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        root.checkVerticalWindow();
      }
      enabled: !root.disableSignals
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
    implicitName: "xmin"
    expression: "-10"
    anchors.left: root.left
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        root.checkHorizontalWindow();
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

  ValueEdit {
    id: xmaxEdit
    implicitName: "xmax"
    expression: "10"
    anchors.right: root.right
    anchors.top: ymaxEdit.bottom
    anchors.margins: spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        root.checkHorizontalWindow();
      }
      enabled: !root.disableSignals
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
    implicitName: "ymin"
    expression: "-10"
    y: xminEdit.y + xminEdit.exprHeight + spacing
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins

    Connections {
      function onValueChanged() {
        root.checkVerticalWindow();
      }
      enabled: !root.disableSignals
    }
  }

  Component.onCompleted: {
    Information.range = graphRange;
  }
}
