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

// The curve buttons on an object's top row: show or hide the curve, unfold its
// plot style, and pick its colors

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

Reveal {
  id: root

  /// @brief null while the controls fade out of an object without a curve
  required property PlotStyle style

  /// @brief shows the end color of the Schrödinger gradient
  required property bool schrodinger

  property alias styleOpen: styleButton.checked

  /// @brief buttons between the style button and the colors
  default property alias extraButtons: extras.data

  CardButton {
    id: displayButton

    checkable: true
    checked: !(root.style?.visible ?? true)

    iconName: checked ? "closed-eye" : "open-eye"

    ToolTip.text: checked ? qsTr("Show the curve") : qsTr("Hide the curve")

    onToggled: {
      if (root.style)
        root.style.visible = !checked
    }
  }

  CardButton {
    id: styleButton

    checkable: true
    checked: false

    iconName: "brush"

    ToolTip.text: qsTr("Plot style")
  }

  RowLayout {
    id: extras
    visible: children.length !== 0
  }

  ColorButton {
    radius: 12

    ToolTip.text: qsTr("The color of the curve")

    // the color stays while the controls fade out
    property themedColor lastGood

    selectedColor: root.style?.color ?? lastGood

    onSelectedColorChanged: {
      if (root.style)
        lastGood = selectedColor;
    }

    onSelectedColorModified: {
      if (root.style)
        root.style.color = selectedColor;
    }
  }

  Reveal {
    shown: root.schrodinger

    ColorButton {
      radius: 12

      ToolTip.text: qsTr("End color of the gradient over the family of Schrödinger curves")

      property themedColor lastGood

      selectedColor: root.style?.secondColor ?? lastGood

      onSelectedColorChanged: {
        if (root.style)
          lastGood = selectedColor;
      }

      onSelectedColorModified: {
        if (root.style)
          root.style.secondColor = selectedColor;
      }
    }
  }
}
