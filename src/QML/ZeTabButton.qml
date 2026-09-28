import QtQuick
import QtQuick.Controls.FluentWinUI3

TabButton {
  id: root

  // the style gives every tab its background image's implicit width, 77 px,
  // however short the title
  implicitWidth: implicitContentWidth + leftPadding + rightPadding

  // TabBar splits its width evenly between tabs without a set width, so a long
  // title elides next to a short one. TabBar keeps widths that are set
  width: {
    const bar = TabBar.tabBar as ZeTabBar;
    return bar ? implicitWidth + Math.max(0, bar.availableWidth - bar.titlesWidth) / bar.count
               : implicitWidth;
  }

  // the style puts 12 on each side
  leftPadding: 6
  rightPadding: 6

  contentItem: Text {
    text: root.text
    font: root.font
    color: root.palette.text
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
  }
}
