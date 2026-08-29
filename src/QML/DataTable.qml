// A table of data, with a header per column and per row
import QtQuick
import QtQuick.Controls.FluentWinUI3


Item {
  id: root
  clip: true

  required property var model
  property bool interactive: false

  SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

  // Only contribute an implicit size when the table actually has laid-out
  // content. Otherwise we'd report just the header padding, which causes
  // widthWhenVisible (in MainWindow) to settle to a nonsensical value while
  // the model is empty / mid-relayout.
  implicitWidth: scrollView.implicitWidth > 0
    ? verticalHeader.implicitWidth + scrollView.implicitWidth
    : 0
  implicitHeight: scrollView.implicitHeight > 0
    ? horizontalHeader.implicitHeight + scrollView.implicitHeight
    : 0

  Menu {
    id: menu
    MenuItem {
      text: qsTr("Clear")
      onTriggered: { tableView.clearSelection(); }
      visible: tableView.selectionModel.hasSelection || tableView.selectionModel.currentIndex.valid
    }
    MenuItem {
      text: qsTr("Delete")
      onTriggered: { tableView.deleteSelection(); }
      visible: tableView.selectionModel.hasSelection || tableView.selectionModel.currentIndex.valid
    }
    MenuItem {
      text: qsTr("Insert row above")
      onTriggered: {
        let row = tableView.selectionModel.currentIndex.row;
        root.model.insertRows(row, 1);
      }
      visible: tableView.selectionModel.currentIndex.valid
    }
    MenuItem {
      text: qsTr("Insert row below")
      onTriggered: {
        let row = tableView.selectionModel.currentIndex.row;
        root.model.insertRows(row+1, 1);
      }
      visible: tableView.selectionModel.currentIndex.valid
    }
  }

  ContextMenu.menu: root.interactive ? menu : null

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

      ZeLabel {
        id: horizontalHeaderLabel
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
      implicitHeight: verticalHeaderLabel.implicitHeight + 3
      implicitWidth: verticalHeaderLabel.implicitWidth + 10

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

      ZeLabel {
        id: verticalHeaderLabel
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

    // TableView reports contentWidth/Height = -1 while the model is being
    // (re)laid out; clamp so we never propagate a bogus negative implicit size.
    implicitWidth: Math.max(0, tableView.contentWidth)
    implicitHeight: Math.max(0, tableView.contentHeight)

    TableView {

      id: tableView
      model: root.model

      editTriggers: TableView.AnyKeyPressed

      columnSpacing: 1
      rowSpacing: 1
      clip: true
      pointerNavigationEnabled: true
      keyNavigationEnabled: true

      // so selectionRect drag selects properly (according to docs)
      acceptedButtons: Qt.NoButton
      selectionBehavior: TableView.SelectCells
      resizableRows: false
      resizableColumns: true

      selectionModel: ItemSelectionModel {
        model: tableView.model
      }

      function clearSelection() {
        const currentIndex = selectionModel.currentIndex;
        console.debug("TableEdit: clearing selection: ", selectionModel.selectedIndexes, " current index: ", currentIndex);
        if (selectionModel.hasSelection)
          root.model.clearCells(selectionModel.selectedIndexes);
        if (currentIndex.valid)
          root.model.clearCells([currentIndex]);
      }

      function deleteSelection() {
        const currentIndex = selectionModel.currentIndex;
        if (!selectionModel.hasSelection && !currentIndex.valid)
          return;

        console.debug("TableEdit: deleting selection");

        // Save selected stuff before we start deleting
        const fullColumns = selectionModel.selectedColumns().map(index => index.column);
        const fullRows = selectionModel.selectedRows().map(index => index.row);
        const looseCells = selectionModel.selectedIndexes
          .filter(index => !fullColumns.includes(index.column) && !fullRows.includes(index.row));

        if (currentIndex.valid &&
            !fullColumns.includes(currentIndex.column) &&
            !fullColumns.includes(currentIndex.row) &&
            !looseCells.includes(currentIndex))
          looseCells.push(currentIndex);

        // merge full rows with individual cells in same container, so we can sort it by row
        // we delete the biggest indices first because they won't affect the following ones
        const rowRemovals = [];
        for (const row of fullRows)
          rowRemovals.push({ row: row });
        for (const index of looseCells)
          rowRemovals.push({ row: index.row, column: index.column });

        rowRemovals.sort((a, b) => b.row - a.row);

        for (const item of rowRemovals)
        {
          if ('column' in item)
            root.model.removeCell(index(item.row, item.column));
          else
            root.model.removeRow(item.row);
        }

        // Fully-selected columns: remove the whole Data math object backing each
        for (const column of fullColumns)
          root.model.requestUiInitiatedDelete(column);
      }

      Keys.onPressed: (event)=> {
        console.debug("TableEdit: key pressed, ID=0x", event.key.toString(16));
        if (event.key === Qt.Key_Delete) {
          deleteSelection();
          event.accepted = true;
        } else if (event.key === Qt.Key_Backspace) {
          clearSelection();
          event.accepted = true;
        } else if (event.key == Qt.Key_Return) {
          if (selectionModel.currentIndex.valid)
            edit(selectionModel.currentIndex);
          event.accepted = true;
        }
      }


      Behavior on width {
        NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
      }

      delegate: Item {

        id: item

        required property bool editing
        required property bool selected
        required property bool current
        // row/column are required so they're real `item` properties, reachable
        // as `item.row`/`item.column` from the sibling editDelegate Component.
        required property int row
        required property int column

        onSelectedChanged: {
          let selection = selected ? "selected" : "deselected"
          console.debug("Cell ", selection, ": row=", row, " column=", column);
        }

        implicitWidth: 100
        implicitHeight: displayWidget.implicitHeight

        // TableView caches delegate implicit sizes at layout time and doesn't
        // track later changes — without forceLayout, a cell whose ValueEdit
        // grew (invalid expression error message) keeps its old row height.
        onImplicitHeightChanged: Qt.callLater(TableView.view.forceLayout)

        Expr {
          id: cellBackend
          implicitName: "tableCell"
          expression: display
        }

        ValueEdit {
          id: displayWidget

          backend: cellBackend

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
        }

        TableView.editDelegate: FocusScope {
          width: parent.width
          height: parent.height

          Expr {
            id: editCellBackend
            implicitName: "editCell"
            expression: display
          }

          ValueEdit {
            focus: true
            id: valueEdit
            anchors.fill: parent
            visible: root.interactive && item.editing
            exprEdit.lineEditBackend.textEdit.focus: true
            backend: editCellBackend
          }

          TableView.onCommit: {
            display = valueEdit.expression;
          }

          Component.onCompleted: {
            valueEdit.exprEdit.lineEditBackend.textEdit.selectAll();
          }
        }
      }
    }
  }

  SelectionRectangle {
    id: selectionRect
    target: tableView
    selectionMode: SelectionRectangle.Drag
  }
}
