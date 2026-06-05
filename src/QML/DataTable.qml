// A line edit where the user enters an expression that evaluates to a value
import QtQuick
import QtQuick.Controls.FluentWinUI3


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

  onImplicitHeightChanged: {
    console.debug("DataTable: implicitHeight=", implicitHeight);
  }

  onImplicitWidthChanged: {
    console.debug("DataTable: implicitWidth=", implicitWidth);
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

  ContextMenu.menu: root.interactive ? menu : null

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

      Timer {
        id: advanceEditTimer
        interval: 1
        repeat: false
        property int targetRow: -1
        property int targetCol: -1
        onTriggered: {
          const idx = tableView.index(targetRow, targetCol);
          tableView.selectionModel.setCurrentIndex(idx, ItemSelectionModel.ClearAndSelect);
          tableView.edit(idx);
        }
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

        onCurrentChanged: {
          if (root.interactive && current && !selected)
          {
            console.debug("Making current cell selected")
            tableView.selectionModel.select(tableView.index(row, column), ItemSelectionModel.Select);
          }
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
            if (activeFocus) {
              console.log("Triggering next column cell edit");
              advanceEditTimer.targetRow = item.row + 1;
              advanceEditTimer.targetCol = item.column;
              advanceEditTimer.start();
            }
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
