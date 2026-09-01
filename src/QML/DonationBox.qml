// The site's donation appeal, and the button that opens the donation page.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  spacing: 10

  LinkLabel {
    Layout.fillWidth: true

    markdown: Donation.words(Information.appSettings.language)
  }

  Button {
    Layout.alignment: Qt.AlignCenter

    highlighted: true
    text: qsTr("Donate")

    // the same icon as the site's donation panel. Its viewBox is 576x512, so a
    // box with another ratio would stretch it
    icon.source: "qrc:/icons/hand-holding-heart.svg"
    icon.width: 18
    icon.height: 16

    onClicked: Qt.openUrlExternally(BuildInfo.donationPage)
  }
}
