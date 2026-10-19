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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
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
  onDeleteRequested: root.mathObj.requestUiInitiatedDelete()

  frame.bottomPadding: 10

  /// @brief true for the objects that draw a curve
  readonly property bool plotted: root.mathObj.style !== null

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
            root.mathObj.getDataSheet().addColumn();
        }
      }

      Item {
        Layout.fillWidth: true
      }

      CardButton {
        id: displayButton

        checkable: true
        checked: !(root.mathObj.style?.visible ?? true)

        iconName: checked ? "closed-eye" : "open-eye"

        ToolTip.text: checked ? qsTr("Show the curve") : qsTr("Hide the curve")

        onToggled: {
          if (root.mathObj.style)
            root.mathObj.style.visible = !checked
        }
      }

      CardButton {
        id: styleButton

        checkable: true
        checked: false

        iconName: "brush"

        ToolTip.text: qsTr("Plot style")
      }

      ColorButton {
        id: colorButton
        radius: 12

        ToolTip.text: qsTr("The color of the curve")

        property themedColor lastGood

        selectedColor: root.mathObj.style?.color ?? lastGood

        onSelectedColorChanged: {
          if (root.mathObj.style)
            lastGood = selectedColor;
        }

        onSelectedColorModified: {
          if (root.mathObj.style)
            root.mathObj.style.color = selectedColor;
        }
      }

      ColorButton {
        id: secondColorButton
        radius: 12

        ToolTip.text: qsTr("End color of the gradient over the family of Schrödinger curves")

        property themedColor lastGood

        selectedColor: root.mathObj.style?.secondColor ?? lastGood

        onSelectedColorChanged: {
          if (root.mathObj.style)
            lastGood = selectedColor;
        }

        onSelectedColorModified: {
          if (root.mathObj.style)
            root.mathObj.style.secondColor = selectedColor;
        }

        states: [
          State {
            name: "hidden";
            when: !root.mathObj.schrodinger || root.mathObj.type === MathObject.CONSTANT
            PropertyChanges {
              secondColorButton.opacity: 0.
              secondColorButton.radius: 0
              secondColorButton.visible: false
            }
          },
          State {
            name: "shown";
            when: root.mathObj.schrodinger && root.mathObj.type !== MathObject.CONSTANT
            PropertyChanges {
              secondColorButton.opacity: 1.
              secondColorButton.radius: 12
              secondColorButton.visible: true
            }
          }
        ]

        transitions: commonTransition
      }

      Item {
        Layout.fillWidth: true
      }

      Transition {
        id: commonTransition

        reversible: true
        from: "shown"
        to: "hidden"
        SequentialAnimation {
          NumberAnimation {
            easing.type: Easing.InOutQuad
            properties: "opacity,maximumWidth,maximumHeight,radius"
            duration: 200;
          }
          PropertyAction {
            property: "visible"
          }
        }
      }

      CardButton {
        id: schrodingerCatButton

        readonly property bool isConstant: root.mathObj.type === MathObject.CONSTANT

        checkable: isConstant

        checked: isConstant && root.mathObj.schrodinger

        iconName: "schrodinger-cat"

        ToolTip.text: qsTr("Schrödinger mode: the constant takes several values at once")

        onToggled: {
          root.mathObj.getConstant().deadAndAlive = checked;
        }

        states: [
          State {
            name: "hidden";
            when: !schrodingerCatButton.isConstant
            PropertyChanges {
              schrodingerCatButton.opacity: 0.
              schrodingerCatButton.Layout.maximumWidth: 0
              schrodingerCatButton.visible: false
            }
          },
          State {
            name: "shown";
            when: schrodingerCatButton.isConstant
            PropertyChanges {
              schrodingerCatButton.opacity: 1.
              schrodingerCatButton.Layout.maximumWidth: 30
              schrodingerCatButton.visible: true
            }
          }
        ]

        transitions: commonTransition
      }

      Image {
        Layout.minimumWidth: 20
        Layout.maximumWidth: 30
        Layout.preferredHeight: Layout.preferredWidth
        Layout.preferredWidth: 30

        source: ZeStyle.icon("schrodinger-cat")
        fillMode: Image.PreserveAspectFit
        mipmap: true

        id: schrodingerObject

        states: [
          State {
            name: "hidden";
            when: !root.mathObj.schrodinger || root.mathObj.type === MathObject.CONSTANT
            PropertyChanges {
              schrodingerObject.Layout.maximumWidth: 0
              schrodingerObject.Layout.maximumHeight: 0
              schrodingerObject.visible: false
            }
          },
          State {
            name: "shown";
            when: root.mathObj.schrodinger && root.mathObj.type !== MathObject.CONSTANT
            PropertyChanges {
              schrodingerObject.Layout.maximumWidth: 30
              schrodingerObject.Layout.maximumHeight: 30
              schrodingerObject.visible: true
            }
          }
        ]

        transitions: commonTransition
      }

      DragHandle {
        id: dragHandle

        ToolTip.text: qsTr("Drag to change the order of the objects")
      }

      states: [
        State {
          name: "hidden";
          when: !root.plotted
          PropertyChanges {
            colorButton.opacity: 0.
            colorButton.radius: 0
            colorButton.visible: false

            styleButton.opacity: 0.
            styleButton.Layout.maximumWidth: 0
            styleButton.Layout.maximumHeight: 0
            styleButton.visible: false

            displayButton.opacity: 0.
            displayButton.Layout.maximumWidth: 0
            displayButton.Layout.maximumHeight: 0
            displayButton.visible: false
          }
        },
        State {
          name: "shown";
          when: root.plotted
          PropertyChanges {
            colorButton.opacity: 1.
            colorButton.radius: 12
            colorButton.visible: true

            styleButton.opacity: 1.
            styleButton.Layout.maximumWidth: 30
            styleButton.Layout.maximumHeight: 30
            styleButton.visible: true

            displayButton.opacity: 1.
            displayButton.Layout.maximumWidth: 30
            displayButton.Layout.maximumHeight: 30
            displayButton.visible: true
          }
        }
      ]

      transitions: commonTransition
    }

    ObjectStyle {
      base: root.mathObj.base
      style: root.mathObj.style

      open: styleButton.checked && styleButton.visible
    }

    // getEquation() and the other getters have no notify signal, so each type
    // gets a new item
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
        EquationEdit { backend: root.mathObj.getEquation() }
      }

      Component {
        id: constantEdit
        ConstantEdit { backend: root.mathObj.getConstant() }
      }

      Component {
        id: parametricEdit
        ParametricEdit { backend: root.mathObj.getParametric() }
      }

      Component {
        id: dataSheetEdit
        DataSheetEdit {
          sheet: root.mathObj.getDataSheet()
          onImportCSV: (file, dataSheet) => root.importCSV(file, dataSheet)
        }
      }
    }
  }
}
