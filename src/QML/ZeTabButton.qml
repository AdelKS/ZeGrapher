import QtQuick
import QtQuick.Controls.FluentWinUI3

TabButton {
  id: root

  // the style gives a tab the implicit width of its background image, 77 px,
  // however short its title
  implicitWidth: implicitContentWidth + leftPadding + rightPadding

  // TabBar splits its width evenly between the tabs that have no width set, so
  // a long title elides next to a short one. TabBar keeps a width that is set
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
    font: Information.appSettings.font
    color: root.palette.text
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
  }
}
