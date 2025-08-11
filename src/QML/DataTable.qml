// A line edit where the user enters an expression that evaluates to a value
import QtQuick
import QtQuick.Controls


Item {
  id: root
  clip: true

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  function clearSelection() {
    console.debug("TableEdit: clearing selection");
    if (tableView.selectionModel.hasSelection)
      DataTableModel.clearCells(tableView.selectionModel.selectedIndexes);
  }

  ContextMenu.menu: Menu {
    MenuItem {
      text: qsTr("Clear")
      onTriggered: { clearSelection(); }
      visible: tableView.selectionModel.hasSelection
    }
  }

  Keys.onPressed: (event)=> {
    if ([Qt.Key_Delete, Qt.Key_Backspace].includes(event.key)) {
      clearSelection();
      event.accepted = true;
    }
  }

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
    anchors.top: horizontalHeader.bottom
    anchors.left: verticalHeader.right
    anchors.right: parent.right
    anchors.bottom: parent.bottom

    TableView {

      id: tableView
      model: DataTableModel

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
          if (current && !selected)
          {
            console.debug("Making current cell selected")
            tableView.selectionModel.select(tableView.index(row, column), ItemSelectionModel.Select);
          }
        }

        implicitWidth: 100
        implicitHeight: displayWidget.implicitHeight

        ValueEdit {
          id: displayWidget

          anchors.fill: parent
          implicitName: "tableCell"

          palette.base: item.current
            ? item.palette.accent
            : item.selected
            ? item.palette.light
            : tableView.alternatingRows && item.row % 2 !== 0
            ? item.palette.base
            : item.palette.alternateBase

          visible: !editing
          enabled: false

          expression: { expression = display }
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
            visible: item.editing
            expression: display
            exprEdit.lineEditBackend.textEdit.focus: true
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
