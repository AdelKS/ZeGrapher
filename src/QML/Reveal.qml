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

// A row that fades and narrows out of its layout when 'shown' turns false

import QtQuick
import QtQuick.Layouts

RowLayout {
  property bool shown: true

  // clip only while narrowed: a ColorButton antialiases its disk past its bounds
  clip: Layout.maximumWidth < implicitWidth
  opacity: shown ? 1 : 0
  visible: opacity > 0
  Layout.maximumWidth: shown ? implicitWidth : 0

  Behavior on opacity {
    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
  }

  Behavior on Layout.maximumWidth {
    NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
  }
}
