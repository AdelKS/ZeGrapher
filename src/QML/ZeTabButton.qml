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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
