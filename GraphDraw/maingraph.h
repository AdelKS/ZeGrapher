/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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

#define FUNC_HOVER 0
#define SEQ_HOVER 1
#define TANGENT_RESIZE_HOVER 2
#define TANGENT_MOVE_HOVER 3


class MainGraph : public GraphDraw
{
    Q_OBJECT
public:
    explicit MainGraph(Informations *info);
    ~MainGraph();

    void afficherPtX(double x);   

signals:
    void zoomBoxActive(bool active);    
    void sizeChanged(int H, int W);

public slots:

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

    void determinerCentreEtUnites();
    void placerGraduations();
    void afficherPoint();

    void checkIfActiveSelectionConflicts();

    ExprCalculator *exprCalculator;
    Point lastPosSouris, pointPx, pointUnit;
    QSlider *hSlider, *vSlider;
    QLineEdit *lineX, *lineY;
    double y1, y2, mouseX, mouseY;

    bool dispPoint, boutonPresse, sourisSurUneCourbe,
         dispRectangle, vWidgetState, hWidgetState, xyWidgetsState,
         hHideStarted, vHideStarted, hoveredCurveType, resaveGraph, cancelUpdateSignal,
         resaveTangent, animationUpdate;

    char typeCurseur;   
    int  hBottom, vBottom, xyBottom;
    QTimer timerX, timerY;

    QPolygonF polygon;   
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
