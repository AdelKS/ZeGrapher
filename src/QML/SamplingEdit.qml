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

// Fills the given column with the values of a math object, unfolds and folds back on demand

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

import ZeGrapher as ZG

Foldable {
  id: root

  required property Data column

  signal applied()

  /// @brief a sampling bound or step, under its label
  component Field: ColumnLayout {
    id: field

    required property string label
    required property Expr backend

    spacing: 0
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignTop

    ZeLabel {
      text: field.label
      Layout.alignment: Qt.AlignHCenter
    }

    ValueEdit {
      backend: field.backend
      Layout.fillWidth: true
      Layout.preferredHeight: implicitHeight
    }
  }

  sourceComponent: Frame {
    id: frame

    leftPadding: 10
    rightPadding: 10
    topPadding: 5
    bottomPadding: 5

    readonly property bool fieldsValid:
      sampling.source.state.status === ZG.State.VALID
      && sampling.start.state.status === ZG.State.VALID && !isNaN(sampling.start.value)
      && sampling.end.state.status === ZG.State.VALID && !isNaN(sampling.end.value)
      && sampling.step.state.status === ZG.State.VALID && !isNaN(sampling.step.value)

    Sampling {
      id: sampling
      column: root.column
    }



    ColumnLayout {
      anchors.fill: parent
      spacing: 2

      RowLayout {
        Layout.fillWidth: true
        spacing: 5

        NameEdit {
          id: sourceEdit
          backend: sampling.source
          //: the name of the math object to sample
          label: qsTr("Object")
          ToolTip.text: qsTr("The name of the object to sample.")

          Layout.fillWidth: true
          Layout.preferredWidth: 90
          Layout.alignment: Qt.AlignTop
        }

        Field {
          label: qsTr("Start")
          backend: sampling.start
          Layout.preferredWidth: 50
        }

        Field {
          label: qsTr("End")
          backend: sampling.end
          Layout.preferredWidth: 50
        }

        Field {
          //: the distance x_{i+1} - x_i between two samples f(x_i) and f(x_{i+1}) of the sampled object f
          label: qsTr("Step")
          backend: sampling.step
          Layout.preferredWidth: 30
        }

        Reveal {
          shown: frame.fieldsValid

          Layout.alignment: Qt.AlignTop

          // center on the edit line, below the labels
          Layout.topMargin: sourceEdit.exprY + (sourceEdit.exprHeight - applyButton.height) / 2

          IconRoundButton {
            id: applyButton

            Layout.preferredHeight: 25
            Layout.preferredWidth: 25

            iconName: "check"

            //: tooltip of the button that starts the sampling, once the user has written the object to sample and the range to sample it over
            ToolTip.text: qsTr("Sample")

            onClicked: {
              const issueText = sampling.apply();
              errorLine.text = issueText;

              if (issueText.length === 0)
                root.applied();
            }
          }
        }
      }

      LineEdit {
        id: errorLine

        radius: 4
        border.width: 0
        textEdit.textFormat: TextEdit.RichText
        textEdit.wrapMode: TextEdit.WordWrap
        textEdit.readOnly: true

        visible: textEdit.length !== 0

        Layout.fillWidth: true
        Layout.preferredHeight: visible ? implicitHeight : 0
      }
    }
  }
}
