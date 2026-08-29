// What every release brings since the one that wrote the workspace of the last
// run, and the words of the site that ask for a donation under them.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Dialog {
  id: root

  /// @brief the releases the dialog shows. MainWindow opens it when it holds one
  readonly property alias entries: whatsNew.entries

  title: qsTr("What's new in ZeGrapher %1").arg(Application.version)
  font: Information.appSettings.font

  modal: true
  closePolicy: Popup.CloseOnEscape
  anchors.centerIn: parent

  width: Math.min(560, parent.width - 60)
  height: Math.min(implicitHeight, parent.height - 60)

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  // the title of a Dialog is a line of text, and the logo goes beside it
  header: RowLayout {
    spacing: 12

    Image {
      Layout.leftMargin: 20
      Layout.topMargin: 20

      source: "qrc:/icons/ZeGrapher.svg"
      sourceSize: Qt.size(40, 40)
      fillMode: Image.PreserveAspectFit
      mipmap: true
    }

    Label {
      Layout.fillWidth: true
      Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

      text: root.title
      font.bold: true
      color: myPalette.windowText
      wrapMode: Text.WordWrap
    }
  }

  WhatsNew {
    id: whatsNew

    since: Information.workspaceVersion
    language: Information.appSettings.language
  }

  contentItem: ColumnLayout {
    spacing: 14

    ScrollView {
      id: notesView

      Layout.fillWidth: true
      Layout.fillHeight: true

      // the notes of one release fit without a scrollbar, and a long list of
      // releases scrolls in place of growing the dialog
      Layout.preferredHeight: notes.implicitHeight
      Layout.maximumHeight: 320
      // a layout that fills the height with what is left over leaves nothing
      // when the dialog is short, and the releases have to keep a few lines.
      // The floor never passes the height they ask for, so one short release
      // stays short
      Layout.minimumHeight: Math.min(120, notes.implicitHeight)

      clip: true
      // the scrollbar sits at the right edge of the view, over the padding
      rightPadding: notesView.ScrollBar.vertical.width

      ScrollBar.vertical.policy: ScrollBar.AsNeeded

      ReleaseNotes {
        id: notes

        width: notesView.availableWidth
        entries: whatsNew.entries
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

    Button {
      Layout.alignment: Qt.AlignRight
      text: qsTr("Close")
      onClicked: root.close()
    }
  }
}
