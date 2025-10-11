// A line edit where the user enters an expression that evaluates to a value
import QtQuick
import QtQuick.Controls


Item {
  id: root
  clip: true

  required property var model
  property bool interactive: false

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  function clearSelection() {
    console.debug("TableEdit: clearing selection");
    if (tableView.selectionModel.hasSelection)
      root.model.clearCells(tableView.selectionModel.selectedIndexes);
  }

  implicitHeight: scrollView.implicitHeight + horizontalHeader.implicitHeight

  onImplicitHeightChanged: {
    console.debug("DataTable: implicitHeight=", implicitHeight);
  }

  Menu {
    id: menu
    MenuItem {
      text: qsTr("Clear")
      onTriggered: { clearSelection(); }
      visible: tableView.selectionModel.hasSelection
    }
    MenuItem {
      text: qsTr("Insert row above")
      onTriggered: {
        let row = tableView.currentRow;
        root.model.insertRows(row, 1);
      }
      visible: tableView.currentRow >= 0
    }
    MenuItem {
      text: qsTr("Insert row bellow")
      onTriggered: {
        let row = tableView.currentRow;
        root.model.insertRows(row+1, 1);
      }
      visible: tableView.currentRow >= 0
    }
  }

  ContextMenu.menu: interactive ? menu : null

  Keys.onPressed: (event)=> {
    if ([Qt.Key_Delete, Qt.Key_Backspace].includes(event.key)) {
      clearSelection();
      event.accepted = true;
    }
  }
  Keys.enabled: root.interactive

  HorizontalHeaderView {
    id: horizontalHeader
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.left: verticalHeader.right
    syncView: tableView
    clip: true

    delegate: Rectangle {

      implicitHeight: 25
      color: myPalette.base
      topLeftRadius: 5
      topRightRadius: 5

      TapHandler {
        acceptedModifiers: Qt.NoModifier
        onTapped: {
          console.debug("Clear&Select column: ", column)
          tableView.selectionModel.select(
            tableView.index(0, column),
            ItemSelectionModel.Columns | ItemSelectionModel.ClearAndSelect
          );
        }
        enabled: root.interactive
      }

      TapHandler {
        acceptedModifiers: Qt.ShiftModifier
        onTapped: {
          console.debug("Select column: ", column)
          tableView.selectionModel.select(
            tableView.index(0, column),
            ItemSelectionModel.Columns | ItemSelectionModel.Select
          );
        }
        enabled: root.interactive
      }

      Label {
        anchors.fill: parent
        text: display
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
      }
    }
  }

  VerticalHeaderView {
    id: verticalHeader
    anchors.top: horizontalHeader.bottom
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    resizableRows: false
    syncView: tableView
    clip: true

    delegate: Rectangle {

      color: myPalette.base
      topLeftRadius: 5
      bottomLeftRadius: 5
      implicitHeight: 25

      TapHandler {
        acceptedModifiers: Qt.NoModifier
        onTapped: {
          console.debug("Clear&Select row: ", row)
          tableView.selectionModel.select(
            tableView.index(row, 0),
            ItemSelectionModel.Rows | ItemSelectionModel.ClearAndSelect
          );
        }
        enabled: root.interactive
      }

      TapHandler {
        acceptedModifiers: Qt.ShiftModifier
        onTapped: {
          console.debug("Select row: ", row)
          tableView.selectionModel.select(
            tableView.index(row, 0),
            ItemSelectionModel.Rows | ItemSelectionModel.Select
          );
        }
        enabled: root.interactive
      }

      Label {
        anchors.fill: parent
        text: display
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
      }
    }
  }

  ScrollView {
    id: scrollView

    anchors.top: horizontalHeader.bottom
    anchors.left: verticalHeader.right
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    TableView {

      id: tableView
      model: root.model

      columnSpacing: 1
      rowSpacing: 1
      clip: true
      pointerNavigationEnabled: true
      keyNavigationEnabled: true
      selectionBehavior: TableView.SelectCells
      resizableRows: false
      resizableColumns: true

      selectionModel: ItemSelectionModel {
        model: tableView.model
      }

      Behavior on width {
        NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
      }

      delegate: Item {

        id: item

        required property bool editing
        required property bool selected
        required property bool current

        onSelectedChanged: {
          let selection = selected ? "selected" : "deselected"
          console.debug("Cell ", selection, ": row=", row, " column=", column);
        }

        onCurrentChanged: {
          if (root.interactive && current && !selected)
          {
            console.debug("Making current cell selected")
            tableView.selectionModel.select(tableView.index(row, column), ItemSelectionModel.Select);
          }
        }

        implicitWidth: 100
        implicitHeight: displayWidget.implicitHeight

        ValueEdit {
          id: displayWidget

          backend: MathWorld.addAltExprObject()

          anchors.fill: parent

          palette.base: root.interactive && item.current
            ? item.palette.accent
            : root.interactive && item.selected
            ? item.palette.light
            : tableView.alternatingRows && item.row % 2 !== 0
            ? item.palette.base
            : item.palette.alternateBase

          visible: !root.interactive || !editing
          enabled: false

          expression: { expression = display }

          Component.onCompleted: backend.setImplicitName("tableCell")

          Component.onDestruction : MathWorld.removeAltExprObject(backend)
        }

        TableView.onReused: {
          displayWidget.expression = display;
        }

        TableView.editDelegate: FocusScope {

          width: parent.width
          height: parent.height

          ValueEdit {
            id: valueEdit
            anchors.fill: parent
            visible: root.interactive && item.editing
            expression: display
            exprEdit.lineEditBackend.textEdit.focus: true
            backend: MathWorld.addAltExprObject();

            Component.onCompleted: backend.setImplicitName("editCell")
            Component.onDestruction: MathWorld.removeAltExprObject(backend)
          }

          TableView.onCommit: {
            display = valueEdit.expression;
            displayWidget.expression = valueEdit.expression;
            console.debug("Updating model: display=", valueEdit.expression);
          }

          Component.onCompleted: {
            valueEdit.exprEdit.lineEditBackend.textEdit.selectAll();
          }
        }
      }
    }

    SelectionRectangle {
      id: selectionRect
      target: tableView

      onActiveChanged: {
        console.debug("Selection rectangle state: ", active);
      }

      bottomRightHandle: Rectangle {
        width: 20
        height: 20
        radius: 10
        visible: SelectionRectangle.control.active
      }
      topLeftHandle: Rectangle {
        width: 20
        height: 20
        radius: 10
        visible: SelectionRectangle.control.active
      }
    }
  }


}
