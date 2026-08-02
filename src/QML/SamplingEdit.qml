// Fills the given column with the values of a math object, unfolds and folds back on demand

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.FluentWinUI3

import ZeGrapher as ZG

Loader {
  id: root

  required property Data column

  /// @brief whether the form is unfolded
  property bool open: false

  /// @brief the height currently given to the form, animated
  property real shownHeight: 0

  signal applied()

  active: open || shownHeight > 0

  Layout.fillWidth: true
  Layout.preferredHeight: shownHeight

  clip: true

  visible: shownHeight > 0

  Behavior on shownHeight { SmoothedAnimation { duration: 200 } }

  states: [
    State {
      name: "folded"; when: !root.open
      PropertyChanges { root.shownHeight: 0 }
    },
    State {
      name: "unfolded"; when: root.open
      PropertyChanges {
        explicit: false
        root.shownHeight: root.implicitHeight
      }
    }
  ]

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
          label: qsTr("Object")

          Layout.fillWidth: true
          Layout.preferredWidth: 90
          Layout.alignment: Qt.AlignTop
        }

        ColumnLayout {
          spacing: 0
          Layout.fillWidth: true

          Layout.preferredWidth: 50

          Layout.alignment: Qt.AlignTop

          ZeLabel {
            text: qsTr("Start")
            Layout.alignment: Qt.AlignHCenter
          }

          ValueEdit {
            backend: sampling.start
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
          }
        }

        ColumnLayout {
          spacing: 0
          Layout.fillWidth: true

          Layout.preferredWidth: 50

          Layout.alignment: Qt.AlignTop

          ZeLabel {
            text: qsTr("End")
            Layout.alignment: Qt.AlignHCenter
          }

          ValueEdit {
            backend: sampling.end
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
          }
        }

        ColumnLayout {
          spacing: 0
          Layout.fillWidth: true

          Layout.preferredWidth: 30

          Layout.alignment: Qt.AlignTop

          ZeLabel {
            text: qsTr("Step")
            Layout.alignment: Qt.AlignHCenter
          }

          ValueEdit {
            backend: sampling.step
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
          }
        }

        IconRoundButton {
          id: applyButton

          Layout.preferredHeight: 25
          Layout.preferredWidth: 25
          Layout.alignment: Qt.AlignTop

          // center on the edit line, below the labels
          Layout.topMargin: sourceEdit.exprY + (sourceEdit.exprHeight - height) / 2

          lightThemeIcon: "qrc:/icons/check-dark.svg"
          darkThemeIcon: "qrc:/icons/check-light.svg"

          onReleased: {
            const issueText = sampling.apply();
            errorLine.text = issueText;

            if (issueText.length === 0)
              root.applied();
          }

          states: [
            State {
              name: "hidden"; when: !frame.fieldsValid
              PropertyChanges {
                applyButton.opacity: 0
                applyButton.Layout.maximumWidth: 0
                applyButton.visible: false
              }
            },
            State {
              name: "shown"; when: frame.fieldsValid
              PropertyChanges {
                applyButton.opacity: 1
                applyButton.Layout.maximumWidth: 25
                applyButton.visible: true
              }
            }
          ]

          transitions: Transition {
            reversible: true
            from: "shown"
            to: "hidden"
            SequentialAnimation {
              NumberAnimation {
                easing.type: Easing.InOutQuad
                properties: "opacity,maximumWidth"
                duration: 200
              }
              PropertyAction {
                property: "visible"
              }
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
