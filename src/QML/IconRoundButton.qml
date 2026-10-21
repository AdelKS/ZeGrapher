/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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

import QtQuick
import QtQuick.Controls.FluentWinUI3

RoundButton {
  id: root
  radius: 5

  /// @brief an icon from icons/, without its '-dark' or '-light' suffix
  required property string iconName

  display: Button.IconOnly
  leftPadding: 0
  rightPadding: 0
  topPadding: 0
  bottomPadding: 0

  // an icon has no text, so call sites set the hint with ToolTip.text, which
  // screen readers read too
  ToolTip.delay: ZeStyle.tooltipDelay
  ToolTip.visible: hovered && ToolTip.text.length !== 0
  Accessible.name: ToolTip.text

  contentItem: Image {
    source: ZeStyle.icon(root.iconName)
    fillMode: Image.PreserveAspectFit
    mipmap: true
  }
}
