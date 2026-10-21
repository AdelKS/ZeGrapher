/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
