// The frame of a row of buttons that floats over the bottom of a list

import QtQuick
import QtQuick.Controls.FluentWinUI3

Frame {
  z: 100

  leftPadding: 5
  rightPadding: 5
  topPadding: 5
  bottomPadding: 5

  background: Rectangle {
    color: ZeStyle.palette.light
    radius: 5
    opacity: 0.95
  }
}
