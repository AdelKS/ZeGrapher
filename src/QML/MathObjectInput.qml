/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

ObjectCard {
  id: root

  required property MathObject mathObj

  property alias dragHandle: dragHandle

  signal importCSV(file: url, dataSheet: DataSheet)

  deleteHint: qsTr("Delete this object")

  frame.bottomPadding: 10

  /// @brief true for the objects that draw a curve
  readonly property bool plotted: root.mathObj.style !== null

  /// @brief true for a curve that takes the values of the Schrödinger constant
  readonly property bool schrodingerCurve: root.mathObj.schrodinger && root.mathObj.type !== MathObject.CONSTANT

  ColumnLayout {
    id: layout
    anchors.fill: parent
    spacing: 5

    RowLayout {
      id: firstRow
      Layout.fillWidth: true
      Layout.preferredHeight: 40

      ComboBox {
        id: objectTypeTumbler
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "type"
        background.implicitWidth: implicitContentWidth

        model: [
          { text: qsTr("Equation"), type: MathObject.EQUATION },
          { text: qsTr("Constant"), type: MathObject.CONSTANT },
          { text: qsTr("Parametric"), type: MathObject.PARAMETRIC },
          { text: qsTr("Data"), type: MathObject.DATASHEET },
        ]

        currentIndex: model.findIndex((entry) => entry.type === root.mathObj.type)

        onActivated: {
          if (currentValue === root.mathObj.type)
            return;

          root.mathObj.type = currentValue;
          if (currentValue === MathObject.DATASHEET)
            root.mathObj.dataSheet.addColumn();
        }
      }

      Item {
        Layout.fillWidth: true
      }

      PlotControls {
        id: plotControls

        shown: root.plotted
        style: root.mathObj.style
        schrodinger: root.schrodingerCurve
      }

      Item {
        Layout.fillWidth: true
      }

      Reveal {
        id: constantControls
        shown: root.mathObj.type === MathObject.CONSTANT

        CardButton {
          checkable: constantControls.shown
          checked: constantControls.shown && root.mathObj.schrodinger

          iconName: "schrodinger-cat"

          ToolTip.text: qsTr("Schrödinger mode: the constant takes several values at once")

          onToggled: {
            root.mathObj.constant.deadAndAlive = checked;
          }
        }
      }

      SchrodingerBadge {
        shown: root.schrodingerCurve
      }

      DragHandle {
        id: dragHandle

        ToolTip.text: qsTr("Drag to change the order of the objects")
      }
    }

    ObjectStyle {
      base: root.mathObj.base
      style: root.mathObj.style

      open: plotControls.styleOpen && plotControls.shown
    }

    Loader {
      Layout.fillWidth: true

      sourceComponent: {
        switch (root.mathObj.type) {
          case MathObject.EQUATION: return equationEdit;
          case MathObject.CONSTANT: return constantEdit;
          case MathObject.PARAMETRIC: return parametricEdit;
          case MathObject.DATASHEET: return dataSheetEdit;
        }
      }

      Component {
        id: equationEdit
        EquationEdit { backend: root.mathObj.equation }
      }

      Component {
        id: constantEdit
        ConstantEdit { backend: root.mathObj.constant }
      }

      Component {
        id: parametricEdit
        ParametricEdit { backend: root.mathObj.parametric }
      }

      Component {
        id: dataSheetEdit
        DataSheetEdit {
          sheet: root.mathObj.dataSheet
          onImportCSV: (file, dataSheet) => root.importCSV(file, dataSheet)
        }
      }
    }
  }
}
