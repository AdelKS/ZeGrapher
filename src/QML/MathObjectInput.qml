import QtQuick
import QtQuick.Controls.FluentWinUI3
import QtQuick.Layouts

Item {
  id: root

  required property MathObject mathObj

  property alias dragHandle: dragHandle

  signal importCSV(file: url, dataSheet: DataSheet)

  implicitWidth: frame.implicitWidth + del.height / 2
  implicitHeight: frame.implicitHeight + del.height / 2

  Frame {
    id: frame
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.rightMargin: del.width / 2
    anchors.top: parent.top
    anchors.topMargin: del.height / 2
    anchors.bottom: parent.bottom

    leftPadding: 10
    rightPadding: 10
    topPadding: 5
    bottomPadding: 10

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

          model: ListModel {
            id: eqTypeModel
            ListElement {
              text: qsTr("Equation")
              type: MathObject.EQUATION
              has_graph: true
            }
            ListElement {
              text: qsTr("Constant")
              type: MathObject.CONSTANT
              has_graph: false
            }
            ListElement {
              text: qsTr("Parametric")
              type: MathObject.PARAMETRIC
              has_graph: true
            }
            ListElement {
              text: qsTr("Data")
              type: MathObject.DATASHEET
              has_graph: false
            }
          }

          Connections {
            target: mathObj
            function onTypeChanged() {
              objectTypeTumbler.sync();
            }
          }

          function sync() {
            let currentType = eqTypeModel.get(objectTypeTumbler.currentIndex).type
            if (mathObj.type !== currentType)
            {
              for (var i = 0; i != eqTypeModel.count; i++) {
                if (mathObj.type === model.get(i).type) {
                  currentIndex = i;
                  break;
                }
              }
            }
          }

          onActivated: {
            let newType = eqTypeModel.get(objectTypeTumbler.currentIndex).type
            if (mathObj.type !== newType)
            {
              mathObj.type = newType;
              if (mathObj.type == MathObject.DATASHEET)
                mathObj.getDataSheet().addColumn();
            }
          }
        }

        Item {
          Layout.fillWidth: true
        }

        IconRoundButton {
          id: displayButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          checkable: true
          checked: !(root.mathObj.style?.visible ?? true)

          lightThemeIcon: checked ? "qrc:/icons/closed-eye.svg" : "qrc:/icons/open-eye.svg"
          darkThemeIcon: checked ? "qrc:/icons/closed-eye-light.svg" : "qrc:/icons/open-eye-light.svg"

          ToolTip.text: checked ? qsTr("Show the curve") : qsTr("Hide the curve")

          onToggled: {
            if (root.mathObj.style)
              root.mathObj.style.visible = !checked
          }
        }

        IconRoundButton {
          id: styleButton
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          checkable: true
          checked: false

          lightThemeIcon: "qrc:/icons/brush.svg"
          darkThemeIcon: "qrc:/icons/brush-light.svg"

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

        Connections {
          target: root.mathObj

          function onSchrodingerChanged() {
            if (loader.item && loader.item.animatedConstant)
            {
              schrodingerCatButton.checked = root.mathObj.schrodinger;
            }
          }
        }

        IconRoundButton {
          id: schrodingerCatButton

          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          readonly property bool isConstant: root.mathObj.type === MathObject.CONSTANT

          checkable: isConstant

          checked: isConstant && root.mathObj.schrodinger

          lightThemeIcon: "qrc:/icons/schrodinger-cat-dark.svg"
          darkThemeIcon: "qrc:/icons/schrodinger-cat-light.svg"

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

          source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/schrodinger-cat-dark.svg" : "qrc:/icons/schrodinger-cat-light.svg"
          fillMode: Image.PreserveAspectFit
          mipmap: true

          id: schrodingerObject

          readonly property bool isSchrodingerObject: false

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

        Item {
          id: dragHandle
          Layout.minimumWidth: 20
          Layout.maximumWidth: 30
          Layout.preferredHeight: Layout.preferredWidth
          Layout.preferredWidth: 30

          HoverHandler { id: dragHandleHover }

          ToolTip.delay: ZeStyle.tooltipDelay
          ToolTip.text: qsTr("Drag to change the order of the objects")
          ToolTip.visible: dragHandleHover.hovered

          Image {
            anchors.fill: parent
            source: Application.styleHints.colorScheme === Qt.Light ? "qrc:/icons/drag-handle-dark.svg" : "qrc:/icons/drag-handle-light.svg"
            fillMode: Image.PreserveAspectFit
            mipmap: true
          }
        }

        states: [
          State {
            name: "hidden";
            when: ! eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
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
            when: eqTypeModel.get(objectTypeTumbler.currentIndex).has_graph
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

      Loader {
        id: loader
        Layout.fillWidth: true

        property int currentType: -1

        Connections {
          target: mathObj
          function onTypeChanged() {
            loader.sync();
          }
        }

        function sync() {
          if (currentType !== mathObj.type)
          {
            currentType = mathObj.type
            if (currentType === MathObject.EQUATION) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/EquationEdit.qml", {"backend": mathObj.getEquation()});
            } else if (currentType === MathObject.CONSTANT) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/ConstantEdit.qml", {"backend": mathObj.getConstant()});
            } else if (currentType === MathObject.PARAMETRIC) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/ParametricEdit.qml", {"backend": mathObj.getParametric()});
            } else if (currentType === MathObject.DATASHEET) {
              loader.setSource("qrc:/qt/qml/ZeGrapher/DataSheetEdit.qml", {"sheet": mathObj.getDataSheet()});
            }
          }
        }
      }

      Connections {
        target: loader.item
        ignoreUnknownSignals: true
        function onImportCSV(file: url, dataSheet: DataSheet) { root.importCSV(file, dataSheet); }
      }
    }
  }


  IconRoundButton {
    id: del
    width: 15
    height: 15
    anchors.horizontalCenter: frame.right
    anchors.horizontalCenterOffset: -2
    anchors.verticalCenter: frame.top
    anchors.verticalCenterOffset: 2
    z: 1

    lightThemeIcon: "qrc:/icons/delete-dark.svg"
    darkThemeIcon: "qrc:/icons/delete-light.svg"

    ToolTip.text: qsTr("Delete this object")

    onReleased: root.mathObj.requestUiInitiatedDelete();
  }

  Component.onCompleted: {
    loader.sync();
    objectTypeTumbler.sync();
  }

}
