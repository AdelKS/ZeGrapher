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

#include "GraphDraw/maingraph.h"


MainGraph::MainGraph(Information *info) : BaseGraphDraw(info)
{
    
    setMinimumSize(QSize(200, 200));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(info, SIGNAL(updateOccured()), this, SLOT(updateGraph()));
    connect(info, SIGNAL(newGraphSettings()), this, SLOT(updateGraph()));
    connect(info, SIGNAL(dataUpdated()), this, SLOT(updateData()));
    connect(info, SIGNAL(animationUpdate()), this, SLOT(updateParEq()));
    connect(info, SIGNAL(gridStateChange()), this, SLOT(updateGraph()));
    connect(info, SIGNAL(drawStateUpdateOccured()), this, SLOT(reactivateSmoothing()));

    exprCalculator = new ExprCalculator(false, info->getFuncsList());

    selectedCurve.isSomethingSelected = false;
    cancelUpdateSignal = false;
    resaveTangent = animationUpdate = false;

    mouseNotOnHWidget.setInterval(1000);
    mouseNotOnHWidget.setSingleShot(true);

    connect(&mouseNotOnHWidget, SIGNAL(timeout()), &hWidgetHideTransition, SLOT(start()));

    mouseNotOnVWidget.setInterval(1000);
    mouseNotOnVWidget.setSingleShot(true);

    connect(&mouseNotOnVWidget, SIGNAL(timeout()), &vWidgetHideTransition, SLOT(start()));

    hWidgetHideTransition.setInterval(30);
    vWidgetHideTransition.setInterval(30);

    hWidgetShowTransition.setInterval(30);
    vWidgetShowTransition.setInterval(30);

    connect(&hWidgetHideTransition, SIGNAL(timeout()), this, SLOT(hideHorWidget()));
    connect(&vWidgetHideTransition, SIGNAL(timeout()), this, SLOT(hideVerWidget()));
    connect(&hWidgetShowTransition, SIGNAL(timeout()), this, SLOT(showHorWidget()));
    connect(&vWidgetShowTransition, SIGNAL(timeout()), this, SLOT(showVerWidget()));

    timeWaitForXYWidgets.setInterval(1000);
    timeWaitForXYWidgets.setSingleShot(true);

    connect(&timeWaitForXYWidgets, SIGNAL(timeout()), &xyWidgetsHideTransition, SLOT(start()));

    xyWidgetsShowTransition.setInterval(30);
    xyWidgetsHideTransition.setInterval(30);

    connect(&xyWidgetsShowTransition, SIGNAL(timeout()), this, SLOT(showXYWidgets()));
    connect(&xyWidgetsHideTransition, SIGNAL(timeout()), this, SLOT(hideXYWidgets()));


    repaintTimer.setInterval(100);
    repaintTimer.setSingleShot(true);
    connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(reactivateSmoothing()));

    timerX.setInterval(35);
    connect(&timerX, SIGNAL(timeout()), this, SLOT(zoomX()));

    timerY.setInterval(35);
    connect(&timerY, SIGNAL(timeout()), this, SLOT(zoomY()));

    dispPoint = false;
    ongoingMouseClick = false;

    dispRectangle = recalculate = recalculateRegs = false;
    hHideStarted = vHideStarted = xyWidgetsState = false;
    mouseState.pointedObjectType = NONE;
    moving = false;

    kLabel.setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #FFFFFF, stop: 0.3 #D0D0D0 , stop: 0.75 #FFFFFF, stop: 1 #FFFFFF);"
                         " border-width: 1px; border-color: #D0D0D0; border-style: solid; border-radius: 10;");

    customFunctions << "f(x) =" << "g(x) =" << "h(x) =" << "p(x) =" << "r(x) =" << "m(x) =";
    customSequences << "u<sub>n</sub> =" << "v<sub>n</sub> =" << "l<sub>n</sub> =" << "w<sub>n</sub> =" << "q<sub>n</sub> =" << "t<sub>n</sub> =";

    kLabel.setParent(this);
    kLabel.hide();

    savedGraph = nullptr;

    setMouseTracking(true);
    cursorType = NORMAL;

    addOtherWidgets();
}

void MainGraph::reactivateSmoothing()
{
    moving = recalculate = false;
    resaveGraph = true;
    update();
}

void MainGraph::updateParEq()
{
    animationUpdate = true;
    repaint();
}

void MainGraph::updateGraph()
{
    if(!cancelUpdateSignal)
    {
        resaveGraph = true;
        recalculate = true;
        update();
    }
    cancelUpdateSignal = false;

}

void MainGraph::updateData()
{
    resaveGraph = true;
    recalculate = false;
    recalculateRegs = true;
    update();
}

void MainGraph::addOtherWidgets()
{
    QLabel *zoom1 = new QLabel();
    zoom1->setFixedSize(22,22);
    zoom1->setScaledContents(true);
    zoom1->setPixmap(QPixmap(":/icons/zoom-in-icon.png"));

    QLabel *zoom2 = new QLabel();
    zoom2->setFixedSize(22,22);
    zoom2->setScaledContents(true);
    zoom2->setPixmap(QPixmap(":/icons/zoom-in-icon.png"));

    QLabel *unZoom1 = new QLabel();
    unZoom1->setFixedSize(22,22);
    unZoom1->setScaledContents(true);
    unZoom1->setPixmap(QPixmap(":/icons/zoom-out-icon.png"));

    QLabel *unZoom2 = new QLabel();
    unZoom2->setFixedSize(22,22);
    unZoom2->setScaledContents(true);
    unZoom2->setPixmap(QPixmap(":/icons/zoom-out-icon.png"));

    xTextLabel = new QLabel();
    xTextLabel->setFixedHeight(25);
    xTextLabel->setMaximumWidth(35);
    xTextLabel->setMinimumWidth(25);

    yTextLabel = new QLabel();
    yTextLabel->setFixedHeight(25);
    yTextLabel->setMaximumWidth(45);
    yTextLabel->setMinimumWidth(35);

    hSlider = new QSlider(Qt::Horizontal);
    hSlider->setFixedSize(250,20);
    hSlider->setMinimum(-50);
    hSlider->setMaximum(50);
    hSlider->setTickPosition(QSlider::TicksBothSides);
    hSlider->setFocusPolicy(Qt::NoFocus);
    hSlider->setValue(0);

    connect(hSlider, SIGNAL(sliderPressed()), &timerX, SLOT(start()));
    connect(hSlider, SIGNAL(sliderReleased()), this, SLOT(stop_X_zoom()));

    vSlider = new QSlider(Qt::Vertical);
    vSlider->setFixedSize(20,250);
    vSlider->setMinimum(-50);
    vSlider->setMaximum(50);
    vSlider->setTickPosition(QSlider::TicksBothSides);
    vSlider->setFocusPolicy(Qt::NoFocus);
    vSlider->setValue(0);

    connect(vSlider, SIGNAL(sliderPressed()), &timerY, SLOT(start()));
    connect(vSlider, SIGNAL(sliderReleased()), this, SLOT(stop_Y_Zoom()));

    lineX = new QLineEdit();
    lineX->setFixedSize(100,25);
    connect(lineX, SIGNAL(returnPressed()), this, SLOT(lineXReturnPressed()));

    lineY = new QLineEdit();
    lineY->setReadOnly(true);
    lineY->setFixedSize(100,25);

    QVBoxLayout *verLayout = new QVBoxLayout();
    verLayout->addWidget(zoom1);
    verLayout->addWidget(vSlider);
    verLayout->addWidget(unZoom1);

    vWidget = new QWidget(this);
    vWidget->setLayout(verLayout);

    QHBoxLayout *horLayout = new QHBoxLayout();
    horLayout->addWidget(unZoom2);
    horLayout->addWidget(hSlider);
    horLayout->addWidget(zoom2);


    hWidget = new QWidget(this);
    hWidget->setLayout(horLayout);

    xWidget = new QWidget(this);
    xWidget->setStyleSheet("border-top-right-radius: 8; border-top-left-radius: 8");
    QHBoxLayout *hbox1 = new QHBoxLayout();
    hbox1->addStretch();
    hbox1->addWidget(xTextLabel);
    hbox1->addWidget(lineX);
    hbox1->addStretch();
    xWidget->setLayout(hbox1);
    xWidget->setAutoFillBackground(true);


    yWidget = new QWidget(this);
    yWidget->setStyleSheet("border-top-right-radius: 8; border-top-left-radius: 8");
    QHBoxLayout *hbox2 = new QHBoxLayout();
    hbox2->addStretch();
    hbox2->addWidget(yTextLabel);
    hbox2->addWidget(lineY);
    hbox2->addStretch();
    yWidget->setLayout(hbox2);
    yWidget->setAutoFillBackground(true);

    vWidget->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #FFFFFF, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #FFFFFF); "
                           "border-width: 1px; border-color: #D0D0D0; border-style: solid; border-style: solid; border-top-left-radius: 10; border-bottom-left-radius: 10;");

    hWidget->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #FFFFFF); "
                           "border-width: 1px; border-color: #D0D0D0; border-style: solid; border-top-left-radius: 10; border-top-right-radius: 10;");
    hSlider->setStyleSheet("background-color: none; border: none;");
    zoom2->setStyleSheet("background-color: none; border: none;");
    unZoom2->setStyleSheet("background-color: none; border: none;");
    vSlider->setStyleSheet("background-color: none; border: none;");
    zoom1->setStyleSheet("background-color: none; border: none;");
    unZoom1->setStyleSheet("background-color: none; border: none;");
}

void MainGraph::hideHorWidget()
{
    if(hWidget->y() >= hBottom)
    {
        hWidgetHideTransition.stop();
        hWidget->hide();
        hWidgetState = false;
        hHideStarted = false;
    }
    else hWidget->move(hWidget->x(), hWidget->y()+6);
}

void MainGraph::hideVerWidget()
{
    if(vWidget->x() >= vBottom)
    {
        vWidgetHideTransition.stop();
        vWidget->hide();
        vWidgetState = false;
        vHideStarted = false;
    }
    else vWidget->move(vWidget->x()+6, vWidget->y());
}

void MainGraph::hideXYWidgets()
{
    xWidget->move(xWidget->x(), xWidget->y()+6);
    yWidget->move(yWidget->x(), yWidget->y()+6);
    if(xWidget->y() >= xyBottom)
    {
        xyWidgetsState = false;
        xyWidgetsHideTransition.stop();
        xWidget->hide();
        yWidget->hide();
    }
}

void MainGraph::showHorWidget()
{
    hWidget->show();

    if(hWidget->y() <= hTopLeft.y())
    {
        hWidgetState = true;
        hWidgetShowTransition.stop();

    }
    else hWidget->move(hWidget->x(), hWidget->y()-6);
}

void MainGraph::showVerWidget()
{
    vWidget->show();
    if(vWidget->x() <= vTopLeft.x())
    {
        vWidgetState = true;
        vWidgetShowTransition.stop();
    }
    else vWidget->move(vWidget->x()-6, vWidget->y());
}

void MainGraph::showXYWidgets()
{
    xWidget->show();
    yWidget->show();
    xWidget->move(xWidget->x(), xWidget->y()-6);
    yWidget->move(yWidget->x(), yWidget->y()-6);
    if(xWidget->y() <= xTopLeft.y())
    {
        xyWidgetsState = true;
        xyWidgetsShowTransition.stop();
    }
}

void MainGraph::lineXReturnPressed()
{
    bool ok = true;
    double val = exprCalculator->calculateExpression(lineX->text(), ok);

    if(ok)
        afficherPtX(val);
}

void MainGraph::wheelEvent(QWheelEvent *event)
{
    repaintTimer.stop();

    double x = event->pos().x();
    double y = event->pos().y();

    x = (x-centre.x)/uniteX;
    y = -(y-centre.y)/uniteY;

    double ratio = tanh((double)(event->angleDelta().y()) / 1024) / 1.1;

    graphView.zoomView(QPointF(x,y), ratio);
    information->setGraphRange(graphView.getGraphRange());

    event->accept();

    //the graph will update from the "information" class updateoccured() signal
}

void MainGraph::afficherPtX(double x)
{
    if(selectedCurve.isSomethingSelected && !selectedCurve.tangentSelection)
    {
        if(selectedCurve.selectedObject == FUNCTION)
        {
            double k = 0;

            if(selectedCurve.isParametric)
            {
                Range info = funcs[selectedCurve.id]->getParametricRange();
                k = info.start + double(selectedCurve.kPos) * info.step;
            }

            pointUnit.x = x;
            pointUnit.y = funcs[selectedCurve.id]->getFuncValue(x, k);

            dispPoint = true;
        }
        else if(selectedCurve.selectedObject == SEQUENCE)
        {
            if(trunc(x) >= seqs[selectedCurve.id]->get_nMin())
            {
                bool ok = true;
                x = trunc(x);
                pointUnit.x = x;
                pointUnit.y = seqs[selectedCurve.id]->getSeqValue(x, ok, selectedCurve.kPos);
                dispPoint = true;
            }
        }

        update();

    }
}

void MainGraph::newWindowSize()
{
    if(hWidget != nullptr)
    {
        hTopLeft.setX((width()-hWidget->width())/2);
        hTopLeft.setY(height()-hWidget->height());

        hBottom = hTopLeft.y()+hWidget->height()+5;

        xTopLeft.setX(hTopLeft.x()-xWidget->width());
        xTopLeft.setY(hTopLeft.y());

        yTopLeft.setX(hTopLeft.x()+hWidget->width());
        yTopLeft.setY(hTopLeft.y());

        vTopLeft.setX(width()-vWidget->width());
        vTopLeft.setY((height()-vWidget->height())/2);

        xyBottom = yTopLeft.y()+yWidget->height()+5;

        vBottom = vTopLeft.x()+vWidget->width()+5;

        yWidget->move(yTopLeft);
        xWidget->move(xTopLeft);

        if(!xyWidgetsState && !selectedCurve.isSomethingSelected)
            timeWaitForXYWidgets.start();

        vWidget->move(vTopLeft);
        hWidget->move(hTopLeft);

        hWidgetRect.setTopLeft(hTopLeft);
        hWidgetRect.setHeight(hWidget->height());
        hWidgetRect.setWidth(hWidget->width());

        vWidgetRect.setTopLeft(vTopLeft);
        vWidgetRect.setHeight(vWidget->height());
        vWidgetRect.setWidth(vWidget->width());

        hWidgetState = true;
        vWidgetState = true;

    }
}

void MainGraph::paintEvent(QPaintEvent *event)
{
    if(windowSize != size())
    {
       newWindowSize();
       resaveGraph = true;
       recalculate = true;
    }

    if(!moving and (cursorType == NORMAL or animationUpdate))
        indirectPaint();
    else directPaint();

    event->accept();
}

void MainGraph::indirectPaint()
{
    if(resaveTangent)
        addTangentToBuffer();
    if(resaveGraph)
        resaveImageBuffer();

    painter.begin(this);

    painter.setFont(information->getGraphSettings().graphFont);

    painter.drawImage(QPoint(0,0), *savedGraph);
    painter.translate(QPointF(centre.x, centre.y));
    painter.scale(1/uniteX, -1/uniteY);

    drawAnimatedParEq();
    drawData();
    animationUpdate = false;

    if(dispPoint)
        drawPoint();

    if(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection)
        drawOneTangent(selectedCurve.id);

    if(mouseState.pointedObjectType != SelectableMathObject::NONE)
        drawHoveringConsequence();

    painter.end();

}

void MainGraph::directPaint()
{
    resaveGraph = true;

    painter.begin(this);
    //trace du background

    painter.setFont(information->getGraphSettings().graphFont);

    painter.setBrush(QBrush(graphSettings.backgroundColor));

    painter.drawRect(-1, -1, graphWidth+1, graphHeight+1);

    updateCenterPosAndScaling();

    painter.translate(QPointF(centre.x, centre.y));
    painter.scale(1/uniteX, -1/uniteY);

    drawAxes();
    drawBaseGraph();

    if(dispRectangle)
    {
        painter.setBrush(Qt::NoBrush);
        pen.setWidth(1);
        pen.setColor(graphSettings.axesColor);
        painter.setPen(pen);
        painter.drawRect(rectReel);
    }

    if(recalculate)
    {
        recalculate = false;
        funcValuesSaver->calculateAll(uniteX, uniteY, graphView);
        recalculateRegVals();
    }
    else if(recalculateRegs)
    {
        recalculateRegs = false;
        recalculateRegVals();
    }

    painter.translate(QPointF(centre.x, centre.y));

    drawFunctions();
    drawSequences();
    drawStraightLines();
    drawTangents();
    drawAllParEq();
    drawRegressions();
    drawData();

    if(dispPoint)
        drawPoint();

    painter.end();
}

void MainGraph::drawHoveringConsequence()
{
    if(mouseState.pointedObjectType == FUNCTION)
    {
        drawCurve(graphSettings.curvesThickness + 1, funcs[mouseState.id]->getColorSaver()->getColor(mouseState.kPos),
                funcValuesSaver->getCurve(mouseState.id, mouseState.kPos));
    }
    else if(mouseState.pointedObjectType == SEQUENCE)
    {
        drawOneSequence(mouseState.id, graphSettings.curvesThickness + 4);
    }
    else if(mouseState.pointedObjectType == TANGENT_MOVE || mouseState.pointedObjectType == TANGENT_RESIZE)
    {
        TangentWidget *tangent = tangents->at(mouseState.id);
        TangentPoints points = tangent->getCaracteristicPoints();

        pen.setWidth(graphSettings.curvesThickness + 4);
        pen.setColor(tangent->getColor());
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing);

        if(mouseState.pointedObjectType == TANGENT_MOVE)
        {
            QPointF pt(points.center.x * uniteX, - points.center.y * uniteY);
            painter.drawPoint(pt);
        }
        else
        {
            QPointF pt1(points.left.x * uniteX, - points.left.y * uniteY);
            painter.drawPoint(pt1);

            QPointF pt2(points.right.x * uniteX, - points.right.y * uniteY);
            painter.drawPoint(pt2);
        }
    }
}

void MainGraph::addTangentToBuffer()
{
    resaveTangent = false;

    painter.begin(savedGraph);

    painter.translate(QPointF(centre.x, centre.y));
    painter.setRenderHint(QPainter::Antialiasing);
    drawOneTangent(tangentDrawException);
    tangentDrawException = -1;

    cancelUpdateSignal = true;
    information->emitUpdateSignal();

    painter.end();
}

void MainGraph::resaveImageBuffer()
{
    resaveGraph = false;

    checkIfActiveSelectionConflicts();

    delete savedGraph;
    savedGraph = new QImage(size(), QImage::Format_RGB32);

    painter.begin(savedGraph);
    //trace du background

    painter.setFont(information->getGraphSettings().graphFont);

    painter.setBrush(QBrush(information->getGraphSettings().backgroundColor));
    painter.drawRect(-1, -1, graphWidthPx+1, graphHeightPx+1);


    updateCenterPosAndScaling();
    drawAxes();
    drawBaseGraph();

    if(recalculate)
    {
        recalculate = false;
        funcValuesSaver->calculateAll(uniteX, uniteY, graphView);
        recalculateRegVals();
    }
    else if(recalculateRegs)
    {
        recalculateRegs = false;
        recalculateRegVals();
    }

    painter.translate(QPointF(centre.x, centre.y));

    drawFunctions();
    drawSequences();
    drawStraightLines();
    drawTangents();
    drawStaticParEq();
    drawRegressions();

    painter.end();
}



void MainGraph::checkIfActiveSelectionConflicts()
{
    if(selectedCurve.isSomethingSelected) //or else there would be problems if a function has been modified (for example, parametric range has changed)
    {
        bool removeSelection = false;

        if(selectedCurve.isParametric)
        {
            if((selectedCurve.selectedObject == FUNCTION && selectedCurve.kPos >= funcValuesSaver->getFuncDrawsNum(selectedCurve.id)) ||
                    (selectedCurve.selectedObject == SEQUENCE && selectedCurve.kPos >= seqs[selectedCurve.id]->getDrawsNum()))
                removeSelection = true;
        }
        if((selectedCurve.selectedObject == FUNCTION && !funcs[selectedCurve.id]->getDrawState()) ||
                    (selectedCurve.selectedObject == SEQUENCE && !seqs[selectedCurve.id]->getDrawState()))
                    removeSelection = true;

        if(removeSelection)
        {
            selectedCurve.isSomethingSelected = false;
            xyWidgetsHideTransition.start();
            kLabel.hide();
        }
    }
}

void MainGraph::drawAllParEq()
{
    drawAnimatedParEq();
    drawStaticParEq();
}

void MainGraph::drawAnimatedParEq()
{
    painter.setRenderHint(QPainter::Antialiasing, information->getGraphSettings().smoothing && !moving);

    QList< QList<Point> > *list;
    QPolygonF polygon;
    ParEqWidget *parWidget;
    ColorSaver *colorSaver;
    Point point;

    pen.setWidth(information->getGraphSettings().curvesThickness);
    painter.setPen(pen);

    int listEnd;

    for(int i = 0; i < parEqs->size(); i++)
    {
        parWidget = parEqs->at(i);
        colorSaver = parWidget->getColorSaver();

        if(parWidget->isAnimated() && parWidget->getDrawState())
        {
            if(parWidget->is_t_Animated())
                list = parWidget->getPointsList();
            else list = parWidget->getCurrentPolygon();

            for(int curve = 0; curve < list->size(); curve++)
            {
                if(!parWidget->is_t_Animated())// equals is_k_animated
                    pen.setColor(colorSaver->getColor(parWidget->getCurrentKPos()));
                else pen.setColor(colorSaver->getColor(curve));

                painter.setPen(pen);

                polygon.clear();

                if(parWidget->is_t_Animated())
                    listEnd = parWidget->getCurrentTPos();
                else listEnd = list->at(curve).size();

                for(int pos = 0 ; pos < listEnd; pos ++)
                {
                    point = list->at(curve).at(pos);
                    polygon << QPointF(point.x * uniteX, - point.y * uniteY);
                }

                if(parWidget->keepTracks() || !parWidget->is_t_Animated())
                    painter.drawPolyline(polygon);

                if(parWidget->is_t_Animated())
                {
                    pen.setWidth(information->getGraphSettings().curvesThickness + 4);
                    painter.setPen(pen);

                    painter.drawPoint(polygon.last());

                    pen.setWidth(information->getGraphSettings().curvesThickness);
                    painter.setPen(pen);
                }
            }
        }
    }
}

void MainGraph::drawPoint()
{
    if(selectedCurve.selectedObject == FUNCTION)
    {
        xTextLabel->setText("x = ");
        yTextLabel->setText(customFunctions[selectedCurve.id]);
    }
    else
    {
        xTextLabel->setText("n = ");
        yTextLabel->setText(customSequences[selectedCurve.id]);
    }

    xTextLabel->setStyleSheet("color: " + information->getViewSettings().axes.x.color.name());
    yTextLabel->setStyleSheet("color: " + information->getViewSettings().axes.y.color.name());



    if(selectedCurve.isParametric)
    {
        Range kRange;
        if(selectedCurve.selectedObject == FUNCTION)
            kRange = funcs[selectedCurve.id]->getParametricRange();
        else kRange = seqs[selectedCurve.id]->getKRange();

        kLabel.move(5, 5);
        double result = kRange.start + selectedCurve.kPos * kRange.step;
        kLabel.setText("k = " + QString::number(result));
        kLabel.adjustSize();
        kLabel.show();
    }

    if(selectedCurve.selectedObject == FUNCTION)
        pen.setColor(funcs[selectedCurve.id]->getColorSaver()->getColor(selectedCurve.kPos));
    else  pen.setColor(seqs[selectedCurve.id]->getColorSaver()->getColor(selectedCurve.kPos));

    int extraWidth = 3;
    if(selectedCurve.selectedObject == SEQUENCE)
        extraWidth += 2;

    pen.setWidth(information->getGraphSettings().curvesThickness + extraWidth);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);

    pointPx.x = pointUnit.x * uniteX;
    pointPx.y = -pointUnit.y * uniteY;

    painter.drawPoint(pointPx.x, pointPx.y);

    lineX->setText(QString::number(pointUnit.x, 'g', NUM_PREC));
    lineY->setText(QString::number(pointUnit.y, 'g', NUM_PREC));

    dispPoint = false;
}

void MainGraph::mousePressEvent(QMouseEvent *event)
{
    ongoingMouseClick = true;
    lastPosSouris.x = event->x();
    lastPosSouris.y = event->y();

    if(event->buttons() & Qt::LeftButton)
    {
        cursorType = NORMAL;

        if(mouseState.pointedObjectType  != SelectableMathObject::NONE && mouseState.tangentHovering)
        {
            selectedCurve.tangentSelection = true;
            selectedCurve.isSomethingSelected = true;
            selectedCurve.tangentPtSelection = mouseState.tangentPtSelection;
            selectedCurve.id = mouseState.id;
            selectedCurve.selectedObject = mouseState.pointedObjectType;
            tangentDrawException = mouseState.id;
            resaveGraph = true;
        }
    }
    else if(event->buttons() & Qt::RightButton)
    {
        cursorType = ZOOMBOX;
        dispRectangle = true;
        rectReel.setTopLeft(QPoint(event->x(), event->y()));
    }
}

void MainGraph::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ongoingMouseClick = false;

    if(cursorType == NORMAL)
    {
        if(mouseState.pointedObjectType  != SelectableMathObject::NONE && !mouseState.tangentHovering)
        {
            selectedCurve.tangentSelection = mouseState.tangentHovering;
            selectedCurve.isSomethingSelected = true;
            selectedCurve.isParametric = mouseState.isParametric;
            selectedCurve.kPos = mouseState.kPos;
            selectedCurve.id = mouseState.id;
            selectedCurve.selectedObject = mouseState.pointedObjectType;
            selectedCurve.tangentPtSelection = mouseState.tangentPtSelection;
            moving = false;

            if(selectedCurve.tangentSelection)
            {
                selectedCurve.isSomethingSelected = false;
                tangentDrawException = selectedCurve.id;
                resaveGraph = true;
            }
            else if(!xyWidgetsState)
                xyWidgetsShowTransition.start();
        }
        else
        {
            if(selectedCurve.isSomethingSelected && !moving)
            {
                if(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection)
                    resaveTangent = true;

                selectedCurve.isSomethingSelected = false;
                xyWidgetsHideTransition.start();
                kLabel.hide();

            }
            else moving = false;

            update();
        }
    }
    else if(cursorType == ZOOMBOX)
    {
        dispRectangle = false;
        rectReel = rectReel.normalized();

        if(rectReel.height() > 10 && rectReel.width() > 10)
        {
            QRectF viewRect;
            viewRect.setRight((rectReel.right() - centre.x) / uniteX);
            viewRect.setLeft((rectReel.left() - centre.x) / uniteX);
            viewRect.setTop((- rectReel.top() + centre.y) / uniteY);
            viewRect.setBottom((- rectReel.bottom() + centre.y) / uniteY);

            if(viewRect.width() > MIN_RANGE && viewRect.height() > MIN_RANGE)
                graphView.setViewRect(viewRect);

            cursorType = NORMAL;
        }
    }
}

void MainGraph::mouseMoveEvent(QMouseEvent *event)
{
    bool refresh = false;

    mouseX = event->x();
    mouseY = event->y();

    if(hWidgetRect.contains(event->pos()))
    {
        hWidgetHideTransition.stop();
        hHideStarted = false;
        hWidgetShowTransition.start();
    }
    else if(hWidgetState && !hHideStarted)
    {
        mouseNotOnHWidget.start();
        hHideStarted = true;
    }

    if(vWidgetRect.contains(event->pos()))
    {
        vWidgetHideTransition.stop();
        vHideStarted = false;
        vWidgetShowTransition.start();
    }
    else if(vWidgetState && !vHideStarted)
    {
        mouseNotOnVWidget.start();
        vHideStarted = true;
    }

    if(cursorType == NORMAL)
    {
        double x = graphView.viewToUnitX((mouseX - centre.x) / uniteX);
        double y = graphView.viewToUnitY(- (mouseY - centre.y) / uniteY);

        if(selectedCurve.isSomethingSelected)
        {
            mouseMoveWithActiveSelection(x, y);
            refresh = true;
        }

        mouseState.pointedObjectType = NONE;

        mouseTangentHoverTest(x, y);

        if(mouseState.pointedObjectType == SelectableMathObject::NONE)
        {
            mouseSeqHoverTest(x, y);
            refresh = refresh || mouseState.pointedObjectType  != SelectableMathObject::NONE;
        }

        if(mouseState.pointedObjectType == SelectableMathObject::NONE)
        {
            mouseFuncHoverTest(x, y);
            refresh = refresh || mouseState.pointedObjectType  != SelectableMathObject::NONE;
        }

        if(mouseState.pointedObjectType == SelectableMathObject::NONE && mouseState.id != -1)
        {
            mouseState.id = -1;
            refresh = true;
        }

        if(ongoingMouseClick && !(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection))
        {
            double dx = -(mouseX - lastPosSouris.x)/uniteX;
            double dy = (mouseY - lastPosSouris.y)/uniteY;

            cancelUpdateSignal = true;
            graphView.translateView(QPointF(dx, dy));         

            updateCenterPosAndScaling();

            if(dx != 0)
            {
                funcValuesSaver->move(graphView);
                moveSavedRegsValues();
            }


            moving = true;
            refresh = true;
        }
    }
    else if(cursorType == ZOOMBOX)
    {
        rectReel.setBottomRight(QPoint(event->x(), event->y()));
        refresh = true;
    }

    lastPosSouris.x = event->x();
    lastPosSouris.y = event->y();

    if(refresh)
        update();



}

void MainGraph::moveSavedRegsValues()
{
    for(auto &reg : regValuesSavers)
        reg.move(graphView);
}

void MainGraph::mouseMoveWithActiveSelection(double x, double y)
{
    Q_UNUSED(y);

    int k_pos = 0;
    double k = 0;

    if(selectedCurve.isParametric)
    {
        if(selectedCurve.selectedObject == FUNCTION)
        {
            Range info = funcs[selectedCurve.id]->getParametricRange();
            k = info.start + double(selectedCurve.kPos) * info.step;
        }
        else k_pos = selectedCurve.kPos;
    }
    if(selectedCurve.selectedObject == FUNCTION)
    {
        pointUnit.x = x;
        pointUnit.y = funcs[selectedCurve.id]->getFuncValue(x, k);

        dispPoint = true;
        recalculate = false;
    }
    else if(selectedCurve.selectedObject == SEQUENCE && x >= seqs[0]->get_nMin()-0.3)
    {
        double start, step = 1;
        bool ok = true;

        if(graphView.getViewRect().left() > seqs[0]->get_nMin())
            start = trunc(graphView.getViewRect().left());
        else start = seqs[0]->get_nMin();

        if(uniteX < 1)
            step = 5*trunc(1/uniteX);

        if(fabs(trunc((x-start)/step) - (x-start)/step) < 0.5*step)
            pointUnit.x = trunc((x-start)/step) * step + start;
        else pointUnit.x = trunc((x-start)/step) * step + start + step;

        pointUnit.y = seqs[selectedCurve.id]->getSeqValue(pointUnit.x, ok, k_pos);

        if(!ok)
        {
            selectedCurve.isSomethingSelected = false;
            return;
        }

        dispPoint = true;
        recalculate = false;
    }
    else if(selectedCurve.selectedObject == TANGENT_RESIZE)
    {
         double dx = (mouseX - lastPosSouris.x)/uniteX;
         tangents->at(selectedCurve.id)->resizeTangent(dx, selectedCurve.tangentPtSelection);
    }
    else if(selectedCurve.selectedObject == TANGENT_MOVE)
    {
         tangents->at(selectedCurve.id)->move(x);
    }
}

void MainGraph::mouseFuncHoverTest(double x, double y)
{
    double calcY = 0;
    double k = 0;
    int draw;
    double rangeLimit;
    Range kRange;

    for(short i = 0; i < funcs.size(); i++)
    {
        if(!funcs[i]->getDrawState())
            continue;

        kRange = funcs[i]->getParametricRange();
        k = kRange.start;
        draw = 0;
        rangeLimit = kRange.start + kRange.step * PAR_DRAW_LIMIT;

        while(k <= kRange.end && k < rangeLimit )
        {
            calcY = funcs[i]->getFuncValue(x, k);

            if(!(std::isnan(calcY) || std::isinf(calcY)))
            {
                if(!(selectedCurve.isSomethingSelected && selectedCurve.selectedObject == FUNCTION && draw == selectedCurve.kPos && i == selectedCurve.id))
                {
                    if((fabs(calcY - y) * uniteY) < information->getGraphSettings().curvesThickness + 1)
                    {
                        mouseState.tangentHovering = false;
                        mouseState.pointedObjectType = FUNCTION;
                        mouseState.isParametric = funcs[i]->isFuncParametric();
                        mouseState.kPos = draw;
                        mouseState.id = i;
                        recalculate = false;

                        return;
                    }
                }
            }
            k += kRange.step;
            draw++;
        }
    }
}

void MainGraph::mouseSeqHoverTest(double x, double y)
{
    double calcY = 0;
    int drawsNum = 1;
    bool ok;
    double nMin = seqs[0]->get_nMin();

    if(x > nMin)
    {
        double intAbscissa = 0;
        double start, step = 1;

        if(graphView.getViewRect().left() > nMin)
            start = trunc(graphView.getViewRect().left());
        else start = nMin;

        if(uniteX < 1)
             step = 5*trunc(1/uniteX);

        if(fabs((trunc((x-start)/step) - (x-start)/step) * uniteX) < double(information->getGraphSettings().curvesThickness) + 2)

            intAbscissa = trunc((x-start)/step) * step + start;

        if(std::isnan(intAbscissa))
            return;

        for(short i = 0; i < 6; i++)
        {
            if(!seqs[i]->isSeqValid())
                continue;

            drawsNum = seqs[i]->getDrawsNum();
            ok = true;

            for(short draw = 0 ; draw < drawsNum ; draw++)
            {
                calcY = seqs[i]->getSeqValue(intAbscissa, ok, draw);

                if(!ok)
                    break;

                if(!(selectedCurve.isSomethingSelected && selectedCurve.selectedObject == SEQUENCE && draw == selectedCurve.kPos && i == selectedCurve.id))
                {
                    if((fabs(calcY - y) * uniteY) < information->getGraphSettings().curvesThickness + 3)
                    {
                        mouseState.tangentHovering = false;
                        mouseState.pointedObjectType = SEQUENCE;
                        mouseState.isParametric = seqs[i]->isSeqParametric();
                        mouseState.kPos = draw;
                        mouseState.id = i;
                        recalculate = false;
                        return;
                    }
                }
            }
        }
    }




}

void MainGraph::mouseTangentHoverTest(double x, double y)
{
    TangentWidget *tangent;
    TangentPoints tangentPoints;

    bool found = false;

    for(int i = 0 ; i < tangents->size() && !found; i++)
    {
        tangent = tangents->at(i);

        if(!tangent->isTangentValid())
            continue;

        tangentPoints = tangent->getCaracteristicPoints();

        if(fabs(tangentPoints.left.x() - x) < 4/uniteX && fabs(tangentPoints.left.y() - y) < 4/uniteY)
        {
            mouseState.tangentHovering = true;
            mouseState.tangentPtSelection = -1; //-1 for left point, so when we add dx to the tangent's lenght, we multiply it by -1
            mouseState.pointedObjectType = TANGENT_RESIZE;
            mouseState.id = i;
            found = true;
            //recalculate = false;
        }
        else if(fabs(tangentPoints.right.x() - x) < 4/uniteX && fabs(tangentPoints.right.y() - y) < 4/uniteY)
        {           
            mouseState.tangentHovering = true;
            mouseState.tangentPtSelection = 1;
            mouseState.pointedObjectType = TANGENT_RESIZE;
            mouseState.id = i;
            found = true;
            //recalculate = false;
        }
        else if(fabs(tangentPoints.center.x() - x) < 5/uniteX && fabs(tangentPoints.center.y() - y) < 5/uniteY)
        {           
            mouseState.tangentHovering = true;
            mouseState.pointedObjectType = TANGENT_MOVE;
            mouseState.id = i;
            found = true;
            //recalculate = false;
        }
    }

    if(found)
        update();
}

void MainGraph::zoomX()
{
    double valeur = (graphRange.Xmax- graphRange.Xmin) * double(hSlider->value()) * 0.0016;

    double ratio = (graphView.getViewRect().right()- graphView.getViewRect().left()) * (double)(hSlider->value()) * 0.0016;

    graphView.zoomXview(ratio);
    moving = true;
    information->setRange(graphView);

    if(graphSettings.smoothing)
        repaintTimer.start();
}

void MainGraph::stop_X_zoom()
{
    timerX.stop();
    hSlider->setValue(0);
}

void MainGraph::zoomY()
{
    double valeur = (graphRange.Ymax - graphRange.Ymin) * double(vSlider->value()) * 0.0016;

    double valeur = (graphView.getViewRect().top() - graphView.getViewRect().bottom()) * (double)(vSlider->value()) * 0.0016;

    if(!information->isOrthonormal())
    {        
        graphView.zoomYview(valeur);
        recalculate = false;
    }
    else
    {
        graphView.zoomXview(valeur);
        recalculate = true;
    }

    moving = true;
    if(graphSettings.smoothing)
        repaintTimer.start();

    information->setRange(graphView);
}

void MainGraph::stop_Y_Zoom()
{
    timerY.stop();
    vSlider->setValue(0);
}

MainGraph::~MainGraph()
{
    delete savedGraph;
    delete exprCalculator;
}
