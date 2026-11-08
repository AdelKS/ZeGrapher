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

// A line edit for a number, whose border shows whether the text is a number

Rectangle {
  id: rec
  property alias textInput: edit
  property double value: NaN

  // Two-way binding: bind `value` to the source, and write edits back from the
  // signal, so user edits don't break the binding:
  //   value: backend.foo
  //   onNumberEdited: (value) => backend.foo = value
  signal numberEdited(real value);

  function parse(text: string): real {
    return text.length === 0 ? NaN : Number(text);
  }

  // 15 significant digits hide the rounding error of a product: 0.2 * 1.5 shows 0.3
  function format(number: real): string {
    return isNaN(number) ? "" : String(Number(number.toPrecision(15)));
  }

  // show value changes that come from outside, but leave the text alone
  // during an edit: "1." parses to 1, and "-" to NaN
  onValueChanged: {
    const typed = parse(edit.text);
    if (typed !== value && !(isNaN(typed) && isNaN(value)))
      edit.text = format(value);
  }
  Component.onCompleted: edit.text = format(value);

  clip: true

  border.width: 1.5
  radius: 4

  border.color: edit.text === "" ? "grey" : ZeStyle.themed(Information.appSettings.validSyntax)

  implicitHeight: Math.max(textMetrics.height, edit.font.pixelSize) + 10 + 2*border.width

  color: palette.base

  TextInput {
    id: edit
    verticalAlignment: TextInput.AlignVCenter
    anchors.fill: parent
    anchors.leftMargin: 6
    anchors.rightMargin: 6
    color: palette.text
    focus: true
    activeFocusOnTab: true
    font: Information.appSettings.font
    text: ""
    // DoubleValidator follows the locale, and expressions use a dot
    validator: RegularExpressionValidator {
      regularExpression: /[+-]?([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)?/
    }

    onTextEdited: rec.numberEdited(rec.parse(text));
  }

  TextMetrics {
    id: textMetrics
    text: "=+-*/_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    font: edit.font
  }

}
