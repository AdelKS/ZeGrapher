// The dialog shown at the first start after an update: every release since the
// one that wrote the document of the last run, and the words of the site that
// ask for a donation under them.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Dialog {
  id: root

  /// @brief the releases to show. MainWindow opens the dialog when this list is
  ///        not empty
  readonly property alias entries: whatsNew.entries

  title: qsTr("What's new in ZeGrapher %1").arg(Application.version)
  font: Information.appSettings.font

  modal: true
  closePolicy: Popup.CloseOnEscape
  anchors.centerIn: parent

  width: Math.min(560, parent.width - 60)
  height: Math.min(implicitHeight, parent.height - 60)

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  // the title of a Dialog is a line of text, so the logo is drawn beside it here
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

    since: Information.lastDocumentVersion
  }

  contentItem: ColumnLayout {
    spacing: 14

    ScrollView {
      id: notesView

      Layout.fillWidth: true
      Layout.fillHeight: true

      // the notes of one release fit without a scrollbar, and a long list of
      // releases scrolls instead of making the dialog taller
      Layout.preferredHeight: notes.implicitHeight
      Layout.maximumHeight: 320
      // filling the height with what is left over leaves nothing when the dialog
      // is short, and the releases need a few lines in every case. This floor
      // never goes above the height they ask for, so one short release stays
      // short
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

    Separator {
      Layout.fillWidth: true
    }

    DonationBox {
      Layout.fillWidth: true
    }

    Button {
      Layout.alignment: Qt.AlignRight
      text: qsTr("Close")
      onClicked: root.close()
    }
  }
}
