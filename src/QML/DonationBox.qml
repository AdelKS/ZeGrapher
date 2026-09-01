// The words of the site that ask for a donation, and the button that opens the
// page they ask for.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  spacing: 10

  Donation {
    id: donation

    language: Information.appSettings.language
  }

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  LinkLabel {
    Layout.fillWidth: true

    markdown: donation.words
    color: myPalette.text

    onLinkActivated: (link) => Qt.openUrlExternally(link)
  }

  Button {
    Layout.alignment: Qt.AlignCenter

    highlighted: true
    text: qsTr("Donate")

    // the same icon the donation panel of the site shows. Its viewBox is
    // 576x512, so a box of another ratio would stretch it
    icon.source: "qrc:/icons/hand-holding-heart.svg"
    icon.width: 18
    icon.height: 16

    onClicked: Qt.openUrlExternally(donation.page)
  }
}
