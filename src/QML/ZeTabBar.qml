// A row of ZeTabButton. Each tab takes the width of its title plus an even share
// of the room left, and the tabs scroll when the bar is narrower than their
// titles.

import QtQuick
import QtQuick.Controls.FluentWinUI3

TabBar {
  id: root

  /// @brief the width of the titles of the tabs, spacing included
  readonly property real titlesWidth: {
    let width = spacing * (count - 1);
    for (let i = 0; i < count; ++i)
      width += itemAt(i).implicitWidth;
    return width;
  }

  // TabBar sums the widths of its tabs, which follow the width of the bar, so
  // the sum makes a binding loop. The style also uses the implicit width of its
  // background image, 470 px, when it is larger
  implicitWidth: titlesWidth + leftPadding + rightPadding

  // the style does not clip the ListView that scrolls the tabs
  clip: true
}
