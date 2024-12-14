///****************************************************************************
//**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
//**
//**  This file is part of ZeGrapher's source code.
//**
//**  ZeGrapher is free software: you may copy, redistribute and/or modify it
//**  under the terms of the GNU Affero General Public License as published by the
//**  Free Software Foundation, either version 3 of the License, or (at your
//**  option) any later version.
//**
//**  This file is distributed in the hope that it will be useful, but
//**  WITHOUT ANY WARRANTY; without even the implied warranty of
//**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//**  General Public License for more details.
//**
//**  You should have received a copy of the GNU General Public License
//**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//**
//****************************************************************************/

//#ifndef MainGraph_H
//#define MainGraph_H

//#include "basegraphdraw.h"


//#define DEFAULT_ZOOM_MULTIPLIER 0.04

//#include "../Widgets/popupwidget.h"

//class SubGraph : public Graph
//{
//    Q_OBJECT
//public:
//    explicit SubGraph();
//    ~SubGraph();

//    void afficherPtX(double x);

//signals:
//    void zoomBoxActive(bool active);

//    void graphRangeChanged(GraphRange range);

//public slots:
//    void setGraphRange(GraphRange range);

//    void reactivateSmoothing();
//    void updateParEq();
//    void updateGraph();
//    void updateData();

//protected slots:

//    void zoomX();
//    void stop_X_zoom();

//    void zoomY();
//    void stop_Y_Zoom();

//    void lineXReturnPressed();

//protected:
//    void incrementTickSpacing(double &spacing, int &currentMultiplier);
//    void decrementTickSpacing(double &spacing, int &currentMultiplier);
//    void paintEvent(QPaintEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
//    void wheelEvent(QWheelEvent *event);
//    void showEvent(QShowEvent *event);
//    void addOtherWidgets();

//    void mouseMoveWithActiveSelection(double x, double y);
//    void mouseFuncHoverTest(double x, double y);
//    void mouseSeqHoverTest(double x, double y);
//    void mouseTangentHoverTest(double x, double y);
//    void resaveImageBuffer();
//    void addTangentToBuffer();
//    void drawHoveringConsequence();

//    void newWindowSize();
//    void directPaint();
//    void indirectPaint();
//    void paintGraph(bool bufferPaint = false);

//    void drawAxes();
//    void drawAnimatedParEq();
//    void drawAllParEq();

//    void updateCenterPosAndScaling();
//    void drawBaseGraph();
//    void drawPoint();

//    void moveSavedRegsValues();

//    void checkIfActiveSelectionConflicts();

//    enum CursorType {NORMAL, DEZOOMER, ZOOMBOX, DEPLACER, NO_CURSOR};
//    enum SelectableMathObject {NONE, FUNCTION, SEQUENCE, TANGENT_RESIZE, TANGENT_MOVE};

//    struct MouseState
//    {
//        bool tangentHovering;
//        bool isParametric;
//        int tangentPtSelection;
//        SelectableMathObject pointedObjectType;
//        int kPos;
//        int id;
//    };

//    struct CurveSelection
//    {
//        bool tangentSelection;
//        bool isSomethingSelected;
//        bool isParametric;
//        SelectableMathObject selectedObject;
//        int tangentPtSelection, id, kPos;
//    };

//    ExprCalculator *exprCalculator;
//    Point lastPosSouris, pointPx, pointUnit;
//    QSlider *hSlider, *vSlider;
//    QLineEdit *lineX, *lineY;
//    double y1, y2, mouseX, mouseY, widestXNumber, screenRefreshRate, zoomMultiplier;

//    bool dispPoint, buttonPresse, mouseOnCurve, ongoingMouseClick,
//         dispRectangle, hoveredCurveType, resaveGraph, cancelUpdateSignal,
//         resaveTangent, animationUpdate;

//    char typeCurseur;
//    int  xyBottom;
//    QTimer timerX, timerY, repaintTimer;

//    QSize windowSize;
//    CurveSelection selectedCurve;
//    MouseState mouseState;

//    QRect rectReel;
//    QImage *savedGraph;
//    QList <QString> customFunctions;
//    QList <QString> customSequences;

//    QLabel *xTextLabel, *yTextLabel;
//    QLabel *kLabel;
//    QWidget *kLabelContainer;

//    PopupWidget *hPopupWidget, *vPopupWidget, *xPopupWidget, *yPopupWidget, *kPopupWidget;

//    Point axesIntersec;
//};

//#endif // MainGraph_H
