// The About panel: what the app is, what each release brought, and what it is
// built on.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root

  required property size globalMenuSize

  implicitWidth: layout.implicitWidth

  WhatsNew {
    id: whatsNew

    // this panel is the changelog of the app, so it lists every release
    everyRelease: true
  }

  UpdateCheck {
    id: checker
  }

  /// @brief what the line under the version says about the update check
  function updateStatus() : string {
    //: %1 is the version of the newest release, such as v4.0.0
    const latest = qsTr("Latest release: %1").arg(checker.latestVersion);

    switch (checker.status) {
      case UpdateCheck.IDLE:
        return "";
      case UpdateCheck.CHECKING:
        return qsTr("Checking...");
      case UpdateCheck.ERROR:
        return qsTr("An error occurred.");
      case UpdateCheck.UPDATE_MAYBE_AVAILABLE:
        return qsTr("Update may be available, see zegrapher.com.") + " " + latest;
      case UpdateCheck.UPDATE_AVAILABLE:
        return qsTr("Update available on zegrapher.com.") + " " + latest;
      case UpdateCheck.UP_TO_DATE:
        return qsTr("You have the latest version.");
      default:
        return qsTr("Unhandled error, please report this issue.");
    }
  }

  // the whole tab scrolls, so the links at the end stay reachable in a short
  // window
  ScrollView {
    id: view

    anchors.fill: parent
    anchors.margins: 10

    clip: true
    contentWidth: view.availableWidth
    // the menu of the panel floats over the bottom of every tab
    contentHeight: layout.implicitHeight + root.globalMenuSize.height + 10
    // the scrollbar sits at the right edge of the view, over the padding
    rightPadding: view.ScrollBar.vertical.width

    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    ColumnLayout {
      id: layout

      width: view.availableWidth
      spacing: 14

      RowLayout {
        Layout.fillWidth: true
        spacing: 10

        Image {
          source: "qrc:/icons/ZeGrapher.svg"
          sourceSize: Qt.size(48, 48)
          fillMode: Image.PreserveAspectFit
          mipmap: true
        }

        ColumnLayout {
          spacing: 2
          Layout.alignment: Qt.AlignVCenter

          Label {
            text: "ZeGrapher " + Application.version
            font.bold: true
          }

          Label {
            Layout.fillWidth: true
            text: root.updateStatus()
            visible: text.length !== 0
            wrapMode: Text.WordWrap
            opacity: 0.7
          }

          LinkLabel {
            Layout.fillWidth: true

            opacity: 0.9
            markdown: "%1 · %2 · %3"
                      .arg("[zegrapher.com](https://zegrapher.com)")
                      .arg("[" + qsTr("Source") + "](" + BuildInfo.repository + ")")
                      .arg("[" + qsTr("Email") + "](mailto:contact@zegrapher.com)")
          }

          Label {
            Layout.fillWidth: true
            text: qsTr("By %1").arg("Adel KARA SLIMANE")
            wrapMode: Text.WordWrap
            opacity: 0.5
          }
        }

        Item { Layout.fillWidth: true }

        IconButton {
          Layout.maximumHeight: 25
          Layout.maximumWidth: 25
          Layout.alignment: Qt.AlignCenter

          lightThemeIcon: 'qrc:/icons/loop.svg'
          darkThemeIcon: 'qrc:/icons/loop-light.svg'

          ToolTip.delay: ZeStyle.tooltipDelay
          ToolTip.text: qsTr('Check for updates')
          ToolTip.visible: hovered

          onReleased: checker.refresh()
        }
      }

      LinkLabel {
        Layout.fillWidth: true

        opacity: 0.8
        //: the link opens the text of the license
        markdown: qsTr("Distributed under the [AGPL-3.0](https://www.gnu.org/licenses/agpl-3.0.html) license.")
              + "\n\n"
              //: %1 is the version of Qt, and the link opens the licensing page of Qt
              + qsTr("Built with [Qt](https://www.qt.io/) %1, under the [LGPL version 3](https://www.qt.io/licensing/) license.").arg(BuildInfo.qtVersion)
              + "\n\n"
              //: the link opens the license of the math font that the app carries
              + qsTr("The embedded \"Latin Modern Math\" font is distributed under the [GUST Font License](#font-license).")

        onAnchorActivated: fontLicense.open()
      }

      Separator {
        Layout.fillWidth: true
      }

      DonationBox {
        Layout.fillWidth: true
      }

      Separator {
        Layout.fillWidth: true
      }

      Label {
        Layout.fillWidth: true
        text: qsTr("What's new")
        font.bold: true
        wrapMode: Text.WordWrap
      }

      ReleaseNotes {
        Layout.fillWidth: true
        entries: whatsNew.entries
      }

      Separator {
        Layout.fillWidth: true
      }
    }
  }

  Dialog {
    id: fontLicense

    title: "Latin Modern Math"
    modal: true
    anchors.centerIn: Overlay.overlay
    width: Math.min(560, Overlay.overlay.width - 40)
    height: Math.min(460, Overlay.overlay.height - 40)
    standardButtons: Dialog.Ok

    contentItem: ScrollView {
      clip: true

      TextArea {
        readOnly: true
        selectByMouse: true
        background: null
        wrapMode: Text.WordWrap
        color: ZeStyle.palette.text
        text: BuildInfo.fontLicense
      }
    }
  }
}
