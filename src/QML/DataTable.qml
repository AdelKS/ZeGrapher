// A line edit where the user enters an expression that evaluates to a value

import QtQuick
import QtQuick.Controls

ScrollView {
  id: root

  TableView {
    model: DataTableModel {}

    columnSpacing: 1
    rowSpacing: 1
    clip: true

    delegate: ValueEdit {

      implicitWidth: 100

      expression: displayStr

      // TableView.editDelegate: ValueEdit {

      //   expression: displayStr

      //   onExpressionChanged: {
      //     displayStr = expression;
      //   }

      //   TableView.onCommit: {
      //     displayStr = expression;
      //     console.log("Updating model: displayStr=", expression);
      //   }
      // }
    }
  }
}
