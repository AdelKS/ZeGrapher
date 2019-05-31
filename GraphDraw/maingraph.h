/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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




#ifndef MainGraph_H
#define MainGraph_H

#include "graphdraw.h"


class MainGraph : public GraphDraw
{
    Q_OBJECT
public:
    explicit MainGraph(Information *info);
    ~MainGraph();

    void afficherPtX(double x);

signals:
    void zoomBoxActive(bool active);    
    void sizeChanged(int H, int W);

    void graphRangeChanged(GraphRange range);
    void graphTickIntervalsChanged(GraphTickIntervals interval);

public slots:
    void setGraphRange(GraphRange range);
    void setGraphTickIntervals(GraphTickIntervals interval);

    void showHorWidget();
    void showVerWidget();
    void showXYWidgets();

    void hideHorWidget();
    void hideVerWidget();
    void hideXYWidgets();
    void reactivateSmoothing();
    void updateParEq();
    void updateGraph();
    void updateData();

protected slots:

    void zoomX();
    void stop_X_zoom();

    void zoomY();
    void stop_Y_Zoom();

    void lineXReturnPressed();

protected:

    void paintEvent(QPaintEvent *event);    
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void addOtherWidgets();

    void mouseMoveWithActiveSelection(double x, double y);
    void mouseFuncHoverTest(double x, double y);
    void mouseSeqHoverTest(double x, double y);
    void mouseTangentHoverTest(double x, double y);
    void resaveImageBuffer();    
    void addTangentToBuffer();
    void drawHoveringConsequence();  

    void newWindowSize();
    void directPaint();
    void indirectPaint();

    void drawAxes();
    void drawAnimatedParEq();  
    void drawAllParEq();

    void updateCenterPosAndScaling();
    void drawGridAndCoordinates();
    void drawPoint();

    void moveSavedRegsValues();

    void checkIfActiveSelectionConflicts();

    enum CursorType {NORMAL, DEZOOMER, ZOOMBOX, DEPLACER, NO_CURSOR};
    enum SelectableMathObject {NONE, FUNCTION, SEQUENCE, TANGENT_RESIZE, TANGENT_MOVE};

    struct MouseState
    {
        bool tangentHovering;
        bool isParametric;
        int tangentPtSelection;
        SelectableMathObject pointedObjectType;
        int kPos;
        int id;
    };

    struct CurveSelection
    {
        bool tangentSelection;
        bool isSomethingSelected;
        bool isParametric;
        SelectableMathObject selectedObject;
        int tangentPtSelection, id, kPos;
    };

    ExprCalculator *exprCalculator;
    Point lastPosSouris, pointPx, pointUnit;
    QSlider *hSlider, *vSlider;
    QLineEdit *lineX, *lineY;
    double y1, y2, mouseX, mouseY, widestXNumber;

    bool dispPoint, ongoingMouseClick, dispRectangle, vWidgetState, hWidgetState, xyWidgetsState,
         hHideStarted, vHideStarted, resaveGraph, cancelUpdateSignal,
         resaveTangent, animationUpdate;

    CursorType cursorType;
    int  hBottom, vBottom, xyBottom;
    QTimer timerX, timerY;

    QSize windowSize;  
    CurveSelection selectedCurve;
    MouseState mouseState;

    QRect rectReel, hWidgetRect, vWidgetRect;
    QImage *savedGraph;
    QList <QString> customFunctions;
    QList <QString> customSequences;

    QLabel *xTextLabel, *yTextLabel;
    QLabel kLabel;

    QWidget *hWidget, *vWidget, *xWidget, *yWidget;
    QTimer mouseNotOnHWidget, mouseNotOnVWidget, vWidgetHideTransition,
           hWidgetHideTransition,vWidgetShowTransition, hWidgetShowTransition,
           xyWidgetsShowTransition, xyWidgetsHideTransition, timeWaitForXYWidgets,
           repaintTimer;

    QPoint hTopLeft, vTopLeft, xTopLeft, yTopLeft;
    Point axesIntersec;   
};

#endif // MainGraph_H
