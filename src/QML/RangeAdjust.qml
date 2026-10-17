/****************************************************************************
**  Copyright (c) 2023, Adel Kara Slimane <adel.ks@zegrapher.com>
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

// The bounds of the view: xmin and xmax side by side, between ymax above and
// ymin below

import QtQuick

Item {
  id: root

  required property GraphRange range

  property int spacing: 5
  implicitHeight: yminEdit.y + yminEdit.height

  /// @brief the name of a bound as rich text, 'x' and 'min' give xmin
  function boundName(axis: string, bound: string): string {
    return "<b>" + axis + "</b><sub>" + bound + "</sub>";
  }

  /// @brief the error under a bound when its axis bounds are inverted
  /// @param axisRange  root.range.x or root.range.y
  /// @param lower  true for the min of the axis
  function inversionError(axisRange, axis: string, lower: bool): string {
    // NaN compares false, so a bound without a value gives no error
    if (!(axisRange.min.value >= axisRange.max.value))
      return "";

    const min = boundName(axis, "min"), max = boundName(axis, "max");

    if (lower)
      //: %1 and %2 are the names of two bounds, for example ymin and ymax
      return qsTr("%1 must be smaller than %2").arg(min).arg(max);

    //: %1 and %2 are the names of two bounds, for example ymax and ymin
    return qsTr("%1 must be greater than %2").arg(max).arg(min);
  }

  component BoundLabel: ZeTextEdit {
    readOnly: true
    selectByMouse: false
    activeFocusOnPress: false
    textFormat: TextEdit.RichText
    horizontalAlignment: TextEdit.AlignHCenter
    verticalAlignment: TextEdit.AlignBottom
    color: ZeStyle.palette.text
  }

  BoundLabel {
    id: ymaxLbl
    text: root.boundName("y", "max")
    anchors.top: root.top
    anchors.horizontalCenter: root.horizontalCenter
    width: parent.width/3
  }

  ValueEdit {
    id: ymaxEdit
    anchors.top: ymaxLbl.bottom
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.y.max
    customErrorMsg: root.inversionError(root.range.y, "y", false)
  }

  BoundLabel {
    text: root.boundName("x", "min")
    anchors.horizontalCenter: xminEdit.horizontalCenter
    anchors.bottomMargin: 5
    anchors.bottom: xminEdit.top
  }

  ValueEdit {
    id: xminEdit
    anchors.left: root.left
    anchors.top: ymaxEdit.bottom
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.x.min
    customErrorMsg: root.inversionError(root.range.x, "x", true)
  }

  BoundLabel {
    text: root.boundName("x", "max")
    anchors.horizontalCenter: xmaxEdit.horizontalCenter
    anchors.bottomMargin: 5
    anchors.bottom: xmaxEdit.top
  }

  ValueEdit {
    id: xmaxEdit
    anchors.right: root.right
    anchors.top: ymaxEdit.bottom
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.x.max
    customErrorMsg: root.inversionError(root.range.x, "x", false)
  }

  BoundLabel {
    text: root.boundName("y", "min")
    anchors.bottom: yminEdit.top
    anchors.bottomMargin: 5
    anchors.horizontalCenter: root.horizontalCenter
  }

  ValueEdit {
    id: yminEdit
    y: xminEdit.y + xminEdit.exprHeight + root.spacing
    anchors.horizontalCenter: root.horizontalCenter
    anchors.margins: root.spacing
    width: parent.width/3 - 2*anchors.margins
    backend: root.range.y.min
    customErrorMsg: root.inversionError(root.range.y, "y", true)
  }
}
