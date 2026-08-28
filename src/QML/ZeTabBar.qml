// A row of ZeTabButtons. Each tab takes its title width plus an even share of
// the leftover space, and the tabs scroll when the bar is narrower than their
// titles.

import QtQuick
import QtQuick.Controls.FluentWinUI3

TabBar {
  id: root

  /// @brief the total width of the tab titles, spacing included
  readonly property real titlesWidth: {
    let width = spacing * (count - 1);
    for (let i = 0; i < count; ++i)
      width += itemAt(i).implicitWidth;
    return width;
  }

  // TabBar sums its tab widths, which follow the bar width, so the sum makes a
  // binding loop. The style also uses its background image's implicit width,
  // 470 px, when that is larger
  implicitWidth: titlesWidth + leftPadding + rightPadding

  // the style does not clip the ListView that scrolls the tabs
  clip: true
}
