import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Dialogs
import QtQuick.Layouts

ApplicationWindow {
  id: win
  width: 800
  height: 600
  visible: true
  font: Information.appFont

  FileDialog {
    id: fileDialog
    currentFolder: StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0]
    fileMode: FileDialog.OpenFile
    options: FileDialog.ReadOnly
    nameFilters: [qsTr("CSV") + " (*.csv)", qsTr("Text") + " (*.txt)", qsTr("Any") + " (*)"]
    visible: false
    onAccepted: dataPane.loadCSV(selectedFile)
  }

  IconButton {
    id: drawer_button
    checkable: true
    checked: true
    z: 100
    x: 5
    y: 5
    width: 25
    height: 25
    flat: true
    lightThemeIcon: 'qrc:/icons/drawer.svg'
    darkThemeIcon: 'qrc:/icons/drawer-light.svg'
  }

  Rectangle {
    id: drawer
    x: 0
    z: 50
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: row.width
    color: myPalette.window

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    states: [
      State {
        name: "hidden"; when: !drawer_button.checked
        PropertyChanges { drawer.x: -drawer.width }
      },
      State {
        name: "shown"; when: drawer_button.checked
        PropertyChanges { drawer.x: 0 }
      }
    ]

    transitions: Transition {
      NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad }
    }

    Row {
      id: row
      anchors.top: parent.top
      anchors.bottom: parent.bottom
      anchors.left: parent.left

      Behavior on width {
        NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
      }

      Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 10
      }

      UserInputPanel {
        id: userInput
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10

        Behavior on width {
          NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
        }

        Timer {
          id: widthResetTimer
          interval: 1000
          repeat: false
          onTriggered: userInput.width = userInput.implicitWidth;
        }

        Component.onCompleted: widthResetTimer.start()

        RowLayout
        {
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.bottom: parent.bottom
          anchors.margins: 5
          height: implicitHeight
          spacing: 5

          Item {
            Layout.fillWidth: true
          }

          IconRoundButton {
            id: importCSV
            z: 100
            Layout.preferredHeight: 35
            Layout.preferredWidth: 35
            Layout.alignment: Qt.AlignRight

            lightThemeIcon: "qrc:/icons/csv-import.svg"
            darkThemeIcon: "qrc:/icons/csv-import-light.svg"

            onReleased: fileDialog.visible = true
          }

          IconRoundButton {
            id: add
            z: 100
            Layout.preferredHeight: 35
            Layout.preferredWidth: 35
            Layout.alignment: Qt.AlignRight
            darkThemeIcon: "qrc:/icons/add-light.svg"
            lightThemeIcon: "qrc:/icons/add.svg"

            onReleased: userInput.addWidget()
          }
        }
      }

      Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 5
      }

      Item {
        id: resizeHandle1
        width: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 100

        ToolSeparator
        {
          orientation: Qt.Vertical
          anchors.fill: parent
        }

        MouseArea {
          anchors.fill: parent

          cursorShape: Qt.SizeHorCursor
          acceptedButtons: Qt.LeftButton

          property real mouseXonPress

          onPressed: {
            mouseXonPress = mouseX;
          }

          onMouseXChanged: {
            var diff = mouseX - mouseXonPress;
            if (drawer.width + diff < win.width) {
              userInput.width += diff;
            }
          }
        }

      }

      DataPane {
        id: dataPane

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10
        anchors.topMargin: 10
        width: 150

        property int widthWhenVisible: 150

        Behavior on width {
          NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
        }
      }

      Item {
        id: resizeHandle2
        width: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 100

        ToolSeparator
        {
          orientation: Qt.Vertical
          anchors.fill: parent
        }

        MouseArea {
          anchors.fill: parent

          cursorShape: Qt.SizeHorCursor
          acceptedButtons: Qt.LeftButton

          property real mouseXonPress

          onPressed: {
            mouseXonPress = mouseX;
          }

          onMouseXChanged: {
            var diff = mouseX - mouseXonPress;
            if (drawer.width + diff < win.width && dataPane.width + diff > 0) {
              dataPane.widthWhenVisible = dataPane.width + diff;
              dataPane.width += diff;
            }
          }
        }
      }

      states: [
        State {
          name: "hidden";
          when: DataTableModel.columns === 0 && !dataPane.importingCSV
          PropertyChanges {
            dataPane.opacity: 0.
            resizeHandle2.opacity: 0.
            dataPane.visible: false
            resizeHandle2.visible: false
            dataPane.width: 0
            resizeHandle2.width: 0
          }
        },
        State {
          name: "shown";
          when: DataTableModel.columns !== 0 || dataPane.importingCSV
          PropertyChanges {
            dataPane.opacity: 1.
            resizeHandle2.opacity: 1.
            dataPane.visible: true
            resizeHandle2.visible: true
            dataPane.width: dataPane.widthWhenVisible
            resizeHandle2.width: 5
          }
        }
      ]

      transitions: Transition {
        reversible: true
        from: "shown"
        to: "hidden"
        SequentialAnimation {
          ParallelAnimation {
            NumberAnimation {
              easing.type: Easing.InOutQuad;
              property: "opacity";
              duration: 250;
            }
            NumberAnimation {
              easing.type: Easing.InOutQuad;
              property: "width";
              duration: 250;
            }
          }
          PropertyAction {
            property: "visible"
          }
        }
      }
    }
  }

  InteractiveGraphView {
    id: interactiveGraph
    x: anchorToDrawer ? drawer.width : 0
    y: 0
    height: win.height
    width: anchorToDrawer ? win.width - drawer.width : win.width

    property bool anchorToDrawer: drawer_button.checked && win.width - drawer.width >= 400

    Behavior on x {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    Behavior on width {
      NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }
  }
}
