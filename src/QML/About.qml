// What the app is, what it brought, and what it is built on.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root

  required property size globalMenuSize

  implicitWidth: layout.implicitWidth

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  WhatsNew {
    id: whatsNew

    // the panel is the changelog of the app, and shows every release
    everyRelease: true
    language: Information.appSettings.language
  }

  UpdateCheck {
    id: checker

    onStatusChanged: {
      switch (status) {
        case UpdateCheck.CHECKING:
          updateText.text = qsTr("Checking..."); break;
        case UpdateCheck.ERROR:
          updateText.text = qsTr("An error occurred."); break;
        case UpdateCheck.UPDATE_MAYBE_AVAILABLE:
          updateText.text = qsTr("Update may be available, see zegrapher.com."); break;
        case UpdateCheck.UPDATE_AVAILABLE:
          updateText.text = qsTr("Update available on zegrapher.com."); break;
        case UpdateCheck.UP_TO_DATE:
          updateText.text = qsTr("You have the latest version."); break;
        case UpdateCheck.IDLE:
          updateText.text = ""; break;
        default:
          updateText.text = qsTr("Unhandled error, please report this issue."); break;
      }

      if (status === UpdateCheck.UPDATE_AVAILABLE
          || status === UpdateCheck.UPDATE_MAYBE_AVAILABLE)
        updateText.text += " " + qsTr("Latest release:") + " " + checker.latestVersion;
    }
  }

  // the whole tab scrolls, so a short panel still reaches the links at the end
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
            id: updateText

            Layout.fillWidth: true
            visible: text.length !== 0
            wrapMode: Text.WordWrap
            opacity: 0.7
          }

          LinkLabel {
            Layout.fillWidth: true

            color: myPalette.text
            opacity: 0.9
            markdown: "%1 · %2 · %3"
                      .arg("[zegrapher.com](https://zegrapher.com)")
                      .arg("[" + qsTr("Source") + "](https://github.com/AdelKS/ZeGrapher)")
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

        color: myPalette.text
        opacity: 0.8
        //: %1 opens the license, %2 the site, %3 the source code, %4 the bug tracker
        markdown: qsTr("Distributed under the [AGPL-3.0](https://www.gnu.org/licenses/agpl-3.0.html) license.")
              + "\n\n"
              //: %1 is the version of Qt, and the link opens the licensing page of Qt
              + qsTr("Built with [Qt](https://www.qt.io/) %1, under the [LGPL version 3](https://www.qt.io/licensing/) license.").arg(BuildInfo.qtVersion)
              + "\n\n"
              //: the link opens the license of the math font that the app carries
              + qsTr("The embedded \"Latin Modern Math\" font is distributed under the [GUST Font License](#font-license).")

        // '#font-license' names no page: it opens the license that the app carries
        onLinkActivated: (link) => {
          if (link === "#font-license")
            fontLicense.open();
          else
            Qt.openUrlExternally(link);
        }
      }

      Rectangle {
        Layout.fillWidth: true
        implicitHeight: ZeStyle.separatorThickness
        color: myPalette.mid
      }

      DonationBox {
        Layout.fillWidth: true
        words: whatsNew.donation
        page: whatsNew.donationUrl
      }

      Rectangle {
        Layout.fillWidth: true
        implicitHeight: ZeStyle.separatorThickness
        color: myPalette.mid
      }

      Label {
        Layout.fillWidth: true
        text: qsTr("What's new")
        font.bold: true
        wrapMode: Text.WordWrap
      }

      ReleaseNotes {
        id: notes

        Layout.fillWidth: true
        entries: whatsNew.entries
      }

      Rectangle {
        Layout.fillWidth: true
        implicitHeight: ZeStyle.separatorThickness
        color: myPalette.mid
      }
    }
  }

  Dialog {
    id: fontLicense

    //: the license of the math font that the app carries
    title: qsTr("Latin Modern Math")
    modal: true
    anchors.centerIn: Overlay.overlay
    width: Math.min(560, root.width - 40)
    height: Math.min(460, root.height - 40)
    standardButtons: Dialog.Ok

    contentItem: ScrollView {
      clip: true

      TextArea {
        readOnly: true
        selectByMouse: true
        background: null
        wrapMode: Text.WordWrap
        color: myPalette.text
        text: BuildInfo.fontLicense
      }
    }
  }
}
