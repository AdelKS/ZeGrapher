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

  function checkHorizontalWindow() {
    console.log("checking x range");
    var oldVal = disableSignals;
    root.disableSignals = true;
    if (xminEdit.value < xmaxEdit.value) {
      Information.range.x.min = xminEdit.value;
      Information.range.x.max = xmaxEdit.value;
      xminEdit.state.setValid();
      xmaxEdit.state.setValid();
    } else {
      console.log('setting x range as invalid');
      xminEdit.state.setInvalid("<b>x</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>x</b><sub>max</sub>");
      xmaxEdit.state.setInvalid("<b>x</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>x</b><sub>min</sub>");
    }
    disableSignals = oldVal;
  }

  function checkVerticalWindow() {
    console.log("checking y range");
    var oldVal = disableSignals;
    root.disableSignals = true;
    if (yminEdit.value < ymaxEdit.value) {
      Information.range.y.min = yminEdit.value;
      Information.range.y.max = ymaxEdit.value;
      yminEdit.state.setValid();
      ymaxEdit.state.setValid();
    } else {
      console.log('setting y range as invalid');
      yminEdit.state.setInvalid("<b>y</b><sub>min</sub>" + qsTr(" must be smaller than ") + "<b>y</b><sub>max</sub>");
      ymaxEdit.state.setInvalid("<b>y</b><sub>max</sub>" + qsTr(" must be greater than ") + "<b>y</b><sub>min</sub>");
    }
    disableSignals = oldVal;
  }

  Connections {
    target: Information
    function onGraphRangeChanged() {
      console.log("new graph range from information singleton")
      root.disableSignals = true;
      ymaxEdit.expression = Number(Information.range.y.max).toLocaleString();
      yminEdit.expression = Number(Information.range.y.min).toLocaleString();
      xmaxEdit.expression = Number(Information.range.x.max).toLocaleString();
      xminEdit.expression = Number(Information.range.x.min).toLocaleString();
      root.disableSignals = false;
    }
    enabled: !root.disableSignals
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
}
