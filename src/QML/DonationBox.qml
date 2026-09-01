// The words of the site that ask for a donation, and the button that opens the
// page they ask for.

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ColumnLayout {
  id: root

  /// @brief the markdown of website/content/<lang>/donation.md
  required property string words

  /// @brief the page that the button opens
  required property url page

  spacing: 10

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  LinkLabel {
    Layout.fillWidth: true

    markdown: root.words
    color: myPalette.text

    onLinkActivated: (link) => Qt.openUrlExternally(link)
  }

  Button {
    Layout.alignment: Qt.AlignCenter

    highlighted: true
    text: qsTr("Donate")

    // the icon that the donation page of the site carries. Its viewBox is
    // 576x512, and a box of another ratio stretches it
    icon.source: "qrc:/icons/hand-holding-heart.svg"
    icon.width: 18
    icon.height: 16

    onClicked: Qt.openUrlExternally(root.page)
  }
}
