/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

RowLayout {
  id: root

  property alias model: tumbler.model
  property real maxWidth: Number.POSITIVE_INFINITY
  property alias currentIndex: tumbler.currentIndex

  implicitHeight: tumbler.implicitHeight
  implicitWidth: selector1.implicitWidth + tumbler.implicitWidth + selector2.implicitWidth + 5

  Image {
    id: selector1
    Layout.alignment: Layout.Center
    source: ZeStyle.icon("selector")
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }

  Tumbler {
    id: tumbler
    enabled: root.enabled
    wrap: true
    Layout.fillHeight: true
    Layout.maximumWidth: root.maxWidth
    visibleItemCount: 3
    model: ListModel {}

    onModelChanged: {
      var newImplicitWidth = 0;
      var biggestHeight = 0
      for (var i = 0; i != model.count; i++) {
        textMetrics.text = model.get(i).txt;
        if (textMetrics.width > newImplicitWidth) {
          newImplicitWidth = textMetrics.width;
        }
        if (textMetrics.height > biggestHeight) {
          biggestHeight = textMetrics.height;
        }
      }
      tumbler.implicitWidth = newImplicitWidth;
      tumbler.implicitHeight = Math.round(biggestHeight*2.2);
    }

    TextMetrics {
      id: textMetrics
    }

    delegate: ZeLabel {
      required property string txt

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      height: contentHeight
      id: txtDelegate
      text: txtDelegate.txt
      opacity: 0.4 + Math.max(0, 1 - Math.abs(Tumbler.displacement)) * (0.1 + 0.5 * enabled)
    }
  }

  Image {
    id: selector2
    Layout.alignment: Layout.Center
    source: ZeStyle.icon("selector-flipped")
    sourceSize.width: 8
    fillMode: Image.PreserveAspectFit
    enabled: root.enabled
    opacity: 0.4 + 0.6 * enabled
  }
}
