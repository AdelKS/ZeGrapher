///****************************************************************************
//**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
//**
//**  This file is part of ZeGrapher's source code.
//**
//**  ZeGrapher is free software: you may copy, redistribute and/or modify it
//**  under the terms of the GNU General Public License as published by the
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

//#include "GraphDraw/subgraph.h"


//SubGraph::SubGraph() : BaseGraphDraw()
//{

//    setMinimumSize(QSize(200, 200));
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

//    connect(info, SIGNAL(updateOccured()), this, SLOT(updateGraph()));
//    connect(info, SIGNAL(newGraphSettings()), this, SLOT(updateGraph()));
//    connect(info, SIGNAL(dataUpdated()), this, SLOT(updateData()));
//    connect(info, SIGNAL(animationUpdate()), this, SLOT(updateParEq()));
//    connect(info, SIGNAL(gridStateChange()), this, SLOT(updateGraph()));
//    connect(info, SIGNAL(drawStateUpdateOccured()), this, SLOT(reactivateSmoothing()));

//    exprCalculator = new ExprCalculator(false, information.getFuncsList());

//    selectedCurve.isSomethingSelected = false;
//    cancelUpdateSignal = false;
//    resaveTangent = animationUpdate = false;

//    repaintTimer.setInterval(100);
//    repaintTimer.setSingleShot(true);
//    connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(reactivateSmoothing()));

//    zoomMultiplier = DEFAULT_ZOOM_MULTIPLIER;

//    screenRefreshRate = 60;

//    timerX.setInterval(1000/screenRefreshRate);
//    connect(&timerX, SIGNAL(timeout()), this, SLOT(zoomX()));

//    timerY.setInterval(1000/screenRefreshRate);
//    connect(&timerY, SIGNAL(timeout()), this, SLOT(zoomY()));

//    dispPoint = false;

//    mouseOnCurve = dispRectangle = recalculate = recalculateRegs = false;
//    moving = false;

//    kLabel = new QLabel();
//    kLabelContainer = new QWidget();

//    QHBoxLayout *layout = new QHBoxLayout();
//    layout->setContentsMargins(10, 4, 10, 6);
//    layout->addWidget(kLabel);
//    kLabelContainer->setLayout(layout);

//    kLabelContainer->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #f4f4f4ff, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #f4f4f4ff); "
//                          " border-style: none; border-bottom-right-radius: 10; border-bottom-left-radius: 10;");

//    kPopupWidget = new PopupWidget(PopupPos::TOP, 0.1, this);
//    kPopupWidget->setWidget(kLabelContainer);
//    kPopupWidget->setState(PopupState::HIDDEN);

//    customFunctions << "f(x) =" << "g(x) =" << "h(x) =" << "p(x) =" << "r(x) =" << "m(x) =";
//    customSequences << "u<sub>n</sub> =" << "v<sub>n</sub> =" << "l<sub>n</sub> =" << "w<sub>n</sub> =" << "q<sub>n</sub> =" << "t<sub>n</sub> =";

//    savedGraph = NULL;

//    ongoingMouseClick = false;

//    setMouseTracking(true);

//    addOtherWidgets();
//}

//void SubGraph::reactivateSmoothing()
//{
//    moving = recalculate = false;
//    resaveGraph = true;
//    update();
//}

//void SubGraph::updateParEq()
//{
//    animationUpdate = true;
//    repaint();
//}

//void SubGraph::updateGraph()
//{
//    if(!cancelUpdateSignal)
//    {
//        resaveGraph = true;
//        recalculate = true;
//        update();
//    }
//    cancelUpdateSignal = false;

//}

//void SubGraph::updateData()
//{
//    resaveGraph = true;
//    recalculate = false;
//    recalculateRegs = true;
//    update();
//}

//void SubGraph::addOtherWidgets()
//{
//    QLabel *zoom1 = new QLabel();
//    zoom1->setFixedSize(22,22);
//    zoom1->setScaledContents(true);
//    zoom1->setPixmap(QPixmap(":/icons/zoom-in-icon.png"));

//    QLabel *zoom2 = new QLabel();
//    zoom2->setFixedSize(22,22);
//    zoom2->setScaledContents(true);
//    zoom2->setPixmap(QPixmap(":/icons/zoom-in-icon.png"));

//    QLabel *unZoom1 = new QLabel();
//    unZoom1->setFixedSize(22,22);
//    unZoom1->setScaledContents(true);
//    unZoom1->setPixmap(QPixmap(":/icons/zoom-out-icon.png"));

//    QLabel *unZoom2 = new QLabel();
//    unZoom2->setFixedSize(22,22);
//    unZoom2->setScaledContents(true);
//    unZoom2->setPixmap(QPixmap(":/icons/zoom-out-icon.png"));

//    xTextLabel = new QLabel();
//    xTextLabel->setFixedHeight(25);
//    xTextLabel->setMaximumWidth(35);
//    xTextLabel->setMinimumWidth(25);

//    yTextLabel = new QLabel();
//    yTextLabel->setFixedHeight(25);
//    yTextLabel->setMaximumWidth(45);
//    yTextLabel->setMinimumWidth(35);

//    hSlider = new QSlider(Qt::Horizontal);
//    hSlider->setFixedSize(250,20);
//    hSlider->setMinimum(-50);
//    hSlider->setMaximum(50);
//    hSlider->setTickPosition(QSlider::TicksBothSides);
//    hSlider->setFocusPolicy(Qt::NoFocus);
//    hSlider->setValue(0);

//    connect(hSlider, SIGNAL(sliderPressed()), &timerX, SLOT(start()));
//    connect(hSlider, SIGNAL(sliderReleased()), this, SLOT(stop_X_zoom()));

//    vSlider = new QSlider(Qt::Vertical);
//    vSlider->setFixedSize(20,250);
//    vSlider->setMinimum(-50);
//    vSlider->setMaximum(50);
//    vSlider->setTickPosition(QSlider::TicksBothSides);
//    vSlider->setFocusPolicy(Qt::NoFocus);
//    vSlider->setValue(0);

//    connect(vSlider, SIGNAL(sliderPressed()), &timerY, SLOT(start()));
//    connect(vSlider, SIGNAL(sliderReleased()), this, SLOT(stop_Y_Zoom()));

//    lineX = new QLineEdit();
//    lineX->setFixedSize(100,25);
//    connect(lineX, SIGNAL(returnPressed()), this, SLOT(lineXReturnPressed()));

//    lineY = new QLineEdit();
//    lineY->setReadOnly(true);
//    lineY->setFixedSize(100,25);

//    QVBoxLayout *verLayout = new QVBoxLayout();
//    verLayout->addWidget(zoom1);
//    verLayout->addWidget(vSlider);
//    verLayout->addWidget(unZoom1);

//    QWidget *vWidget = new QWidget(this);
//    vWidget->setLayout(verLayout);

//    vPopupWidget = new PopupWidget(PopupPos::RIGHT, 0.5, this);
//    vPopupWidget->setWidget(vWidget);

//    // -------- Horizontal zoom slider

//    QHBoxLayout *horLayout = new QHBoxLayout();
//    horLayout->addWidget(unZoom2);
//    horLayout->addWidget(hSlider);
//    horLayout->addWidget(zoom2);

//    QWidget *hWidget = new QWidget(this);
//    hWidget->setLayout(horLayout);

//    hPopupWidget = new PopupWidget(PopupPos::BOTTOM, 0.5, this);
//    hPopupWidget->setWidget(hWidget);

//    //-------- x label widget

//    QWidget *xWidget = new QWidget(this);
//    xWidget->setStyleSheet("border-top-right-radius: 8; border-top-left-radius: 8");
//    QHBoxLayout *hbox1 = new QHBoxLayout();
//    hbox1->addStretch();
//    hbox1->addWidget(xTextLabel);
//    hbox1->addWidget(lineX);
//    hbox1->addStretch();
//    xWidget->setLayout(hbox1);
//    xWidget->setAutoFillBackground(true);
//    xWidget->adjustSize();

//    xPopupWidget = new PopupWidget(PopupPos::BOTTOM, 0.25, this);
//    xPopupWidget->setWidget(xWidget);
//    xPopupWidget->setState(PopupState::HIDDEN);

//    QWidget *yWidget = new QWidget(this);

//    yWidget->setStyleSheet("border-top-right-radius: 8; border-top-left-radius: 8");
//    QHBoxLayout *hbox2 = new QHBoxLayout();
//    hbox2->addStretch();
//    hbox2->addWidget(yTextLabel);
//    hbox2->addWidget(lineY);
//    hbox2->addStretch();
//    yWidget->setLayout(hbox2);
//    yWidget->setAutoFillBackground(true);
//    yWidget->adjustSize();

//    yPopupWidget = new PopupWidget(PopupPos::BOTTOM, 0.75, this);
//    yPopupWidget->setWidget(yWidget);
//    yPopupWidget->setState(PopupState::HIDDEN);

//    vWidget->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #f4f4f4ff, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #f4f4f4ff); "
//                           " border-style: none; border-top-left-radius: 16; border-bottom-left-radius: 16;");

//    hWidget->setStyleSheet("background-color: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f4f4f4ff, stop: 0.4 #E8E8E8 , stop: 0.6 #E8E8E8, stop: 1 #f4f4f4ff); "
//                           "border-style: none; border-top-left-radius: 16; border-top-right-radius: 16;");
//    hSlider->setStyleSheet("background-color: none; border: none;");
//    zoom2->setStyleSheet("background-color: none; border: none;");
//    unZoom2->setStyleSheet("background-color: none; border: none;");
//    vSlider->setStyleSheet("background-color: none; border: none;");
//    zoom1->setStyleSheet("background-color: none; border: none;");
//    unZoom1->setStyleSheet("background-color: none; border: none;");
//}

//void SubGraph::lineXReturnPressed()
//{
//    bool ok = true;
//    double val = exprCalculator->calculateExpression(lineX->text(), ok);

//    if(ok)
//        afficherPtX(val);
//}

//void SubGraph::wheelEvent(QWheelEvent *event)
//{
//    repaintTimer.stop();

//    double x = event->pos().x();
//    double y = event->pos().y();

//    x = (x-centre.x)/pxPerUnit.x;
//    y = -(y-centre.y)/pxPerUnit.y;

//    double multiplier = tanh((double)(event->angleDelta().y()) / 1024) / 1.1;

//    viewMapper.zoomView(QPointF(x, y), multiplier);

//    event->accept();

//    //the graph will update from the "information" class updateoccured() signal
//}

//void SubGraph::afficherPtX(double x)
//{
//    if(selectedCurve.isSomethingSelected && !selectedCurve.tangentSelection)
//    {
//        if(selectedCurve.selectedObject == FUNCTION)
//        {
//            double k = 0;

//            if(selectedCurve.isParametric)
//            {
//                Range info = funcs[selectedCurve.id]->getParametricRange();
//                k = info.start + double(selectedCurve.kPos) * info.step;
//            }

//            pointUnit.x = x;
//            pointUnit.y = funcs[selectedCurve.id]->getFuncValue(x, k);

//            dispPoint = true;
//        }
//        else if(selectedCurve.selectedObject == SEQUENCE)
//        {
//            if(trunc(x) >= seqs[selectedCurve.id]->get_nMin())
//            {
//                bool ok = true;
//                x = trunc(x);
//                pointUnit.x = x;
//                pointUnit.y = seqs[selectedCurve.id]->getSeqValue(x, ok, selectedCurve.kPos);
//                dispPoint = true;
//            }
//        }

//        update();

//    }
//}

//void SubGraph::newWindowSize()
//{
//    emit sizeChanged(size().width(), size().height());
//    windowSize = size();

//    vPopupWidget->updatePositions();
//    hPopupWidget->updatePositions();
//    kPopupWidget->updatePositions();
//    xPopupWidget->updatePositions();
//    yPopupWidget->updatePositions();

//}

//void SubGraph::showEvent(QShowEvent *event)
//{
//    /* Wait for showEvent to determine screen refresh rate
//       to make zooming as smooth as the screen enables.*/

//    BaseGraphDraw::showEvent(event);

//    QWindow *appWindow = window()->windowHandle();
//    if(appWindow != nullptr)
//    {
//        screenRefreshRate = appWindow->screen()->refreshRate();
//        qDebug() << "Screen refresh rate: " << screenRefreshRate;
//    }
//    else qDebug() << "Window handle doesn't exist yet at this point";

//    timerX.setInterval(1000/screenRefreshRate);
//    timerY.setInterval(1000/screenRefreshRate);

//}

//void SubGraph::paintEvent(QPaintEvent *event)
//{
//    if(windowSize != size())
//    {
//       newWindowSize();
//       resaveGraph = true;
//       recalculate = true;
//    }

//    if(!moving && (typeCurseur == NORMAL or animationUpdate))
//    {
//        qDebug() << "indirect paint " << qrand();
//        indirectPaint();
//    }
//    else
//    {
//        qDebug() << "direct paint " << qrand();
//        directPaint();
//    }

//    event->accept();
//}

//void SubGraph::indirectPaint()
//{
//    if(resaveTangent)
//        addTangentToBuffer();
//    if(resaveGraph)
//        resaveImageBuffer();

//    painter.begin(this);

//    painter.setFont(information.getGraphSettings().graphFont);

//    painter.drawImage(QPoint(0,0), *savedGraph);
//    painter.translate(QPointF(centre.x, centre.y));
//    painter.scale(1/pxPerUnit.x, -1/pxPerUnit.y);

//    drawAnimatedParEq();
//    drawData();
//    animationUpdate = false;

//    if(dispPoint)
//        drawPoint();

//    if(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection)
//        drawOneTangent(selectedCurve.id);

//    if(mouseState.pointedObjectType != SelectableMathObject::NONE)
//        drawHoveringConsequence();

//    painter.end();

//}

//void SubGraph::directPaint()
//{
//    resaveGraph = true;

//    painter.begin(this);

//    paintGraph();

//    painter.end();
//}

//void SubGraph::paintGraph(bool bufferPaint)
//{
//    const ZeGraphSettings &graphSettings = information.getGraphSettings();
//    painter.setFont(graphSettings.graphFont);

//    updateCenterPosAndScaling();

//    painter.setBrush(QBrush(graphSettings.backgroundColor));
//    painter.drawRect(-1, -1, size().width()+1, size().height()+1);

//    drawAxes();
//    drawTicksAndNumbers();

//    if(not bufferPaint and dispRectangle)
//    {
//        painter.setBrush(Qt::NoBrush);
//        pen.setWidth(1);
//        pen.setColor(graphSettings.axesColor);
//        painter.setPen(pen);
//        painter.drawRect(rectReel);
//    }

//    if(recalculate)
//    {
//        recalculate = false;
//        funcValuesSaver->calculateAll(pxPerUnit.x, pxPerUnit.y, viewMapper);
//        recalculateRegVals();
//    }
//    else if(recalculateRegs)
//    {
//        recalculateRegs = false;
//        recalculateRegVals();
//    }

//    painter.translate(QPointF(centre.x, centre.y));

//    drawFunctions();
//    drawSequences();
//    drawStraightLines();
//    drawTangents();

//    if(bufferPaint)
//        drawStaticParEq();
//    else drawAllParEq();

//    drawRegressions();
//    drawData();

//    if(not bufferPaint and dispPoint)
//        drawPoint();
//}

//void SubGraph::drawHoveringConsequence()
//{
//    const ZeGraphSettings &graphSettings = information.getGraphSettings();

//    if(mouseState.pointedObjectType == FUNCTION)
//    {
//        drawCurve(graphSettings.curvesThickness + 1, funcs[mouseState.id]->getColorSaver()->getColor(mouseState.kPos),
//                funcValuesSaver->getCurve(mouseState.id, mouseState.kPos));
//    }
//    else if(mouseState.pointedObjectType == SEQUENCE)
//    {
//        drawOneSequence(mouseState.id, graphSettings.curvesThickness + 4);
//    }
//    else if(mouseState.pointedObjectType == TANGENT_MOVE || mouseState.pointedObjectType == TANGENT_RESIZE)
//    {
//        TangentWidget *tangent = tangents->at(mouseState.id);
//        TangentPoints points = tangent->getCaracteristicPoints();

//        pen.setWidth(graphSettings.curvesThickness + 4);
//        pen.setColor(tangent->getColor());
//        painter.setPen(pen);
//        painter.setRenderHint(QPainter::Antialiasing);

//        if(mouseState.pointedObjectType == TANGENT_MOVE)
//        {
//            QPointF pt(points.center.x * pxPerUnit.x, - points.center.y * pxPerUnit.y);
//            painter.drawPoint(pt);
//        }
//        else
//        {
//            QPointF pt1(points.left.x * pxPerUnit.x, - points.left.y * pxPerUnit.y);
//            painter.drawPoint(pt1);

//            QPointF pt2(points.right.x * pxPerUnit.x, - points.right.y * pxPerUnit.y);
//            painter.drawPoint(pt2);
//        }
//    }
//}

//void SubGraph::addTangentToBuffer()
//{
//    resaveTangent = false;

//    painter.begin(savedGraph);

//    painter.translate(QPointF(centre.x, centre.y));
//    painter.setRenderHint(QPainter::Antialiasing);
//    drawOneTangent(tangentDrawException);
//    tangentDrawException = -1;

//    cancelUpdateSignal = true;
//    information.emitUpdateSignal();

//    painter.end();
//}

//void SubGraph::resaveImageBuffer()
//{
//    resaveGraph = false;

//    checkIfActiveSelectionConflicts();

//    delete savedGraph;
//    savedGraph = new QImage(size(), QImage::Format_RGB32);

//    painter.begin(savedGraph);

//    paintGraph(true);

//    painter.end();
//}



//void SubGraph::checkIfActiveSelectionConflicts()
//{
//    if(selectedCurve.isSomethingSelected) //or else there would be problems if a function has been modified (for example, parametric range has changed)
//    {
//        bool removeSelection = false;

//        if(selectedCurve.isParametric)
//        {
//            if((selectedCurve.selectedObject == FUNCTION && selectedCurve.kPos >= funcValuesSaver->getFuncDrawsNum(selectedCurve.id)) ||
//                    (selectedCurve.selectedObject == SEQUENCE && selectedCurve.kPos >= seqs[selectedCurve.id]->getDrawsNum()))
//                removeSelection = true;
//        }
//        if((selectedCurve.selectedObject == FUNCTION && !funcs[selectedCurve.id]->getDrawState()) ||
//                    (selectedCurve.selectedObject == SEQUENCE && !seqs[selectedCurve.id]->getDrawState()))
//                    removeSelection = true;

//        if(removeSelection)
//        {
//            selectedCurve.isSomethingSelected = false;
//            xPopupWidget->hideWidget();
//            yPopupWidget->hideWidget();
//            kPopupWidget->hideWidget();
//        }
//    }
//}

//void SubGraph::drawAllParEq()
//{
//    drawAnimatedParEq();
//    drawStaticParEq();
//}

//void SubGraph::drawAnimatedParEq()
//{
//    painter.setRenderHint(QPainter::Antialiasing, information.getGraphSettings().smoothing && !moving);

//    QList< QList<Point> > *list;
//    QPolygonF polygon;
//    ParEqWidget *parWidget;
//    ColorSaver *colorSaver;
//    Point point;

//    pen.setWidth(information.getGraphSettings().curvesThickness);
//    painter.setPen(pen);

//    int listEnd;

//    for(int i = 0; i < parEqs->size(); i++)
//    {
//        parWidget = parEqs->at(i);
//        colorSaver = parWidget->getColorSaver();

//        if(parWidget->isAnimated() && parWidget->getDrawState())
//        {
//            if(parWidget->is_t_Animated())
//                list = parWidget->getPointsList();
//            else list = parWidget->getCurrentPolygon();

//            for(int curve = 0; curve < list->size(); curve++)
//            {
//                if(!parWidget->is_t_Animated())// equals is_k_animated
//                    pen.setColor(colorSaver->getColor(parWidget->getCurrentKPos()));
//                else pen.setColor(colorSaver->getColor(curve));

//                painter.setPen(pen);

//                polygon.clear();

//                if(parWidget->is_t_Animated())
//                    listEnd = parWidget->getCurrentTPos();
//                else listEnd = list->at(curve).size();

//                for(int pos = 0 ; pos < listEnd; pos ++)
//                {
//                    point = list->at(curve).at(pos);
//                    polygon << QPointF(point.x * pxPerUnit.x, - point.y * pxPerUnit.y);
//                }

//                if(parWidget->keepTracks() || !parWidget->is_t_Animated())
//                    painter.drawPolyline(polygon);

//                if(parWidget->is_t_Animated())
//                {
//                    pen.setWidth(information.getGraphSettings().curvesThickness + 4);
//                    painter.setPen(pen);

//                    painter.drawPoint(polygon.last());

//                    pen.setWidth(information.getGraphSettings().curvesThickness);
//                    painter.setPen(pen);
//                }
//            }
//        }
//    }
//}

//void SubGraph::drawPoint()
//{
//    if(selectedCurve.selectedObject == FUNCTION)
//    {
//        xTextLabel->setText("x = ");
//        yTextLabel->setText(customFunctions[selectedCurve.id]);
//    }
//    else
//    {
//        xTextLabel->setText("n = ");
//        yTextLabel->setText(customSequences[selectedCurve.id]);
//    }

//    xTextLabel->setStyleSheet("color: " + information.getViewSettings().axes.x.color.name());
//    yTextLabel->setStyleSheet("color: " + information.getViewSettings().axes.y.color.name());

//    if(selectedCurve.funcType == FUNC_HOVER)
//        pen.setColor(funcs[selectedCurve.id]->getColorSaver()->getColor(selectedCurve.kPos));
//    else  pen.setColor(seqs[selectedCurve.id]->getColorSaver()->getColor(selectedCurve.kPos));

//    int extraWidth = 3;
//    if(selectedCurve.selectedObject == SEQUENCE)
//        extraWidth += 2;

//    pen.setWidth(information.getGraphSettings().curvesThickness + extraWidth);
//    painter.setPen(pen);
//    painter.setRenderHint(QPainter::Antialiasing);

//    pointPx.x = pointUnit.x * pxPerUnit.x;
//    pointPx.y = -pointUnit.y * pxPerUnit.y;

//    painter.drawPoint(pointPx.x, pointPx.y);

//    lineX->setText(QString::number(pointUnit.x, 'g', NUM_PREC));
//    lineY->setText(QString::number(pointUnit.y, 'g', NUM_PREC));

//    dispPoint = false;
//}

//void SubGraph::mousePressEvent(QMouseEvent *event)
//{
//    ongoingMouseClick = true;
//    lastPosSouris.x = event->x();
//    lastPosSouris.y = event->y();

//    if(event->buttons() & Qt::LeftButton)
//    {
//        cursorType = NORMAL;

//        if(mouseState.pointedObjectType  != SelectableMathObject::NONE && mouseState.tangentHovering)
//        {
//            selectedCurve.tangentSelection = true;
//            selectedCurve.isSomethingSelected = true;
//            selectedCurve.tangentPtSelection = mouseState.tangentPtSelection;
//            selectedCurve.id = mouseState.id;
//            selectedCurve.selectedObject = mouseState.pointedObjectType;
//            tangentDrawException = mouseState.id;
//            resaveGraph = true;
//        }
//    }
//    else if(event->buttons() & Qt::RightButton)
//    {
//        cursorType = ZOOMBOX;
//        dispRectangle = true;
//        rectReel.setTopLeft(QPoint(event->x(), event->y()));
//    }
//}

//void SubGraph::mouseReleaseEvent(QMouseEvent *event)
//{
//    Q_UNUSED(event);
//    ongoingMouseClick = false;

//    if(cursorType == NORMAL)
//    {
//        if(mouseState.pointedObjectType  != SelectableMathObject::NONE && !mouseState.tangentHovering)
//        {
//            selectedCurve.tangentSelection = mouseState.tangentHovering;
//            selectedCurve.isSomethingSelected = true;
//            selectedCurve.isParametric = mouseState.isParametric;
//            selectedCurve.kPos = mouseState.kPos;
//            selectedCurve.id = mouseState.id;
//            selectedCurve.selectedObject = mouseState.pointedObjectType;
//            selectedCurve.tangentPtSelection = mouseState.tangentPtSelection;
//            moving = false;

//            if(selectedCurve.tangentSelection)
//            {
//                selectedCurve.isSomethingSelected = false;
//                tangentDrawException = selectedCurve.id;
//                resaveGraph = true;
//            }
//            else
//            {
//                if(selectedCurve.isParametric)
//                {
//                    Range kRange;
//                    if(selectedCurve.funcType == FUNC_HOVER)
//                        kRange = funcs[selectedCurve.id]->getParametricRange();
//                    else kRange = seqs[selectedCurve.id]->getKRange();

//                    double result = kRange.start + selectedCurve.kPos * kRange.step;
//                    kLabel->setText("k = " + QString::number(result));
//                    kLabel->adjustSize();
//                    kLabelContainer->adjustSize();

//                    kPopupWidget->updatePositions();
//                    kPopupWidget->showWidget();
//                }

//                xPopupWidget->showWidget();
//                yPopupWidget->showWidget();
//            }
//        }
//        else
//        {
//            if(selectedCurve.isSomethingSelected && !moving)
//            {
//                if(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection)
//                    resaveTangent = true;

//                selectedCurve.isSomethingSelected = false;
//                xPopupWidget->hideWidget();
//                yPopupWidget->hideWidget();
//                kPopupWidget->hideWidget();

//            }
//            else moving = false;

//            update();
//        }
//    }
//    else if(cursorType == ZOOMBOX)
//    {
//        dispRectangle = false;
//        rectReel = rectReel.normalized();

//        if(rectReel.height() > 10 && rectReel.width() > 10)
//        {
//            QRectF viewRect;
//            viewRect.setRight((rectReel.right() - centre.x) / pxPerUnit.x);
//            viewRect.setLeft((rectReel.left() - centre.x) / pxPerUnit.x);
//            viewRect.setTop((- rectReel.top() + centre.y) / pxPerUnit.y);
//            viewRect.setBottom((- rectReel.bottom() + centre.y) / pxPerUnit.y);

//            if( (win.Xmax - win.Xmin > MIN_AMPLITUDE and win.Ymax - win.Ymin > MIN_AMPLITUDE) and
//                    (win.Xmax - win.Xmin < MAX_AMPLITUDE and win.Ymax - win.Ymin < MAX_AMPLITUDE))
//            {
//                resaveGraph = true;
//                information.setRange(win);
//            }

//            cursorType = NORMAL;
//        }
//    }
//}

//void SubGraph::mouseMoveEvent(QMouseEvent *event)
//{
//    bool refresh = false;

//    mouseX = event->x();
//    mouseY = event->y();

//    if(hPopupWidget->geometry().contains(mouseX, mouseY))
//       hPopupWidget->showWidget();
//    else hPopupWidget->hideWidget();

//    if(vPopupWidget->geometry().contains(mouseX, mouseY))
//       vPopupWidget->showWidget();
//    else vPopupWidget->hideWidget();

//    if(cursorType == NORMAL)
//    {
//        double x = viewMapper.viewToUnitX((mouseX - centre.x) / pxPerUnit.x);
//        double y = viewMapper.viewToUnitY(- (mouseY - centre.y) / pxPerUnit.y);

//        if(selectedCurve.isSomethingSelected)
//        {
//            mouseMoveWithActiveSelection(x, y);
//            refresh = true;
//        }

//        mouseState.pointedObjectType = NONE;

//        mouseTangentHoverTest(x, y);

//        if(mouseState.pointedObjectType == SelectableMathObject::NONE)
//        {
//            mouseSeqHoverTest(x, y);
//            refresh = refresh || mouseState.pointedObjectType  != SelectableMathObject::NONE;
//        }

//        if(mouseState.pointedObjectType == SelectableMathObject::NONE)
//        {
//            mouseFuncHoverTest(x, y);
//            refresh = refresh || mouseState.pointedObjectType  != SelectableMathObject::NONE;
//        }

//        if(mouseState.pointedObjectType == SelectableMathObject::NONE && mouseState.id != -1)
//        {
//            mouseState.id = -1;
//            refresh = true;
//        }

//        if(ongoingMouseClick && !(selectedCurve.isSomethingSelected && selectedCurve.tangentSelection))
//        {
//            double dx = -(mouseX - lastPosSouris.x)/pxPerUnit.x;
//            double dy = (mouseY - lastPosSouris.y)/pxPerUnit.y;

//            cancelUpdateSignal = true;
//            viewMapper.translateView(QPointF(dx, dy));

//            updateCenterPosAndScaling();

//            if(dx != 0)
//            {
//                funcValuesSaver->move(viewMapper);
//                moveSavedRegsValues();
//            }


//            moving = true;
//            refresh = true;
//        }
//    }
//    else if(cursorType == ZOOMBOX)
//    {
//        rectReel.setBottomRight(QPoint(event->x(), event->y()));
//        refresh = true;
//    }

//    lastPosSouris.x = event->x();
//    lastPosSouris.y = event->y();

//    if(refresh)
//        update();



//}

//void SubGraph::moveSavedRegsValues()
//{
//    for(auto &reg : regValuesSavers)
//        reg.move(viewMapper);
//}

//void SubGraph::mouseMoveWithActiveSelection(double x, double y)
//{
//    Q_UNUSED(y);

//    int k_pos = 0;
//    double k = 0;

//    if(selectedCurve.isParametric)
//    {
//        if(selectedCurve.selectedObject == FUNCTION)
//        {
//            Range info = funcs[selectedCurve.id]->getParametricRange();
//            k = info.start + double(selectedCurve.kPos) * info.step;
//        }
//        else k_pos = selectedCurve.kPos;
//    }
//    if(selectedCurve.selectedObject == FUNCTION)
//    {
//        pointUnit.x = x;
//        pointUnit.y = funcs[selectedCurve.id]->getFuncValue(x, k);

//        dispPoint = true;
//        recalculate = false;
//    }
//    else if(selectedCurve.selectedObject == SEQUENCE && x >= seqs[0]->get_nMin()-0.3)
//    {
//        double start, step = 1;
//        bool ok = true;

//        if(viewMapper.getViewRect().left() > seqs[0]->get_nMin())
//            start = trunc(viewMapper.getViewRect().left());
//        else start = seqs[0]->get_nMin();

//        if(pxPerUnit.x < 1)
//            step = 5*trunc(1/pxPerUnit.x);

//        if(fabs(trunc((x-start)/step) - (x-start)/step) < 0.5*step)
//            pointUnit.x = trunc((x-start)/step) * step + start;
//        else pointUnit.x = trunc((x-start)/step) * step + start + step;

//        pointUnit.y = seqs[selectedCurve.id]->getSeqValue(pointUnit.x, ok, k_pos);

//        if(!ok)
//        {
//            selectedCurve.isSomethingSelected = false;
//            return;
//        }

//        dispPoint = true;
//        recalculate = false;
//    }
//    else if(selectedCurve.selectedObject == TANGENT_RESIZE)
//    {
//         double dx = (mouseX - lastPosSouris.x)/pxPerUnit.x;
//         tangents->at(selectedCurve.id)->resizeTangent(dx, selectedCurve.tangentPtSelection);
//    }
//    else if(selectedCurve.selectedObject == TANGENT_MOVE)
//    {
//         tangents->at(selectedCurve.id)->move(x);
//    }
//}

//void SubGraph::mouseFuncHoverTest(double x, double y)
//{
//    double calcY = 0;
//    double k = 0;
//    int draw;
//    double rangeLimit;
//    Range kRange;

//    for(short i = 0; i < funcs.size(); i++)
//    {
//        if(!funcs[i]->getDrawState())
//            continue;

//        kRange = funcs[i]->getParametricRange();
//        k = kRange.start;
//        draw = 0;
//        rangeLimit = kRange.start + kRange.step * PAR_DRAW_LIMIT;

//        while(k <= kRange.end && k < rangeLimit )
//        {
//            calcY = funcs[i]->getFuncValue(x, k);

//            if(!(std::isnan(calcY) || std::isinf(calcY)))
//            {
//                if(!(selectedCurve.isSomethingSelected && selectedCurve.selectedObject == FUNCTION && draw == selectedCurve.kPos && i == selectedCurve.id))
//                {
//                    if((fabs(calcY - y) * pxPerUnit.y) < information.getGraphSettings().curvesThickness + 1)
//                    {
//                        mouseState.tangentHovering = false;
//                        mouseState.pointedObjectType = FUNCTION;
//                        mouseState.isParametric = funcs[i]->isFuncParametric();
//                        mouseState.kPos = draw;
//                        mouseState.id = i;
//                        recalculate = false;

//                        return;
//                    }
//                }
//            }
//            k += kRange.step;
//            draw++;
//        }
//    }
//}

//void SubGraph::mouseSeqHoverTest(double x, double y)
//{
//    double calcY = 0;
//    int drawsNum = 1;
//    bool ok;
//    double nMin = seqs[0]->get_nMin();

//    if(x > nMin)
//    {
//        double intAbscissa = 0;
//        double start, step = 1;

//        if(viewMapper.getViewRect().left() > nMin)
//            start = trunc(viewMapper.getViewRect().left());
//        else start = nMin;

//        if(pxPerUnit.x < 1)
//             step = 5*trunc(1/pxPerUnit.x);

//        if(fabs((trunc((x-start)/step) - (x-start)/step) * pxPerUnit.x) < double(information.getGraphSettings().curvesThickness) + 2)

//            intAbscissa = trunc((x-start)/step) * step + start;

//        if(std::isnan(intAbscissa))
//            return;

//        for(short i = 0; i < 6; i++)
//        {
//            if(!seqs[i]->isSeqValid())
//                continue;

//            drawsNum = seqs[i]->getDrawsNum();
//            ok = true;

//            for(short draw = 0 ; draw < drawsNum ; draw++)
//            {
//                calcY = seqs[i]->getSeqValue(intAbscissa, ok, draw);

//                if(!ok)
//                    break;

//                if(!(selectedCurve.isSomethingSelected && selectedCurve.selectedObject == SEQUENCE && draw == selectedCurve.kPos && i == selectedCurve.id))
//                {
//                    if((fabs(calcY - y) * pxPerUnit.y) < information.getGraphSettings().curvesThickness + 3)
//                    {
//                        mouseState.tangentHovering = false;
//                        mouseState.pointedObjectType = SEQUENCE;
//                        mouseState.isParametric = seqs[i]->isSeqParametric();
//                        mouseState.kPos = draw;
//                        mouseState.id = i;
//                        recalculate = false;
//                        return;
//                    }
//                }
//            }
//        }
//    }
//}

//void SubGraph::mouseTangentHoverTest(double x, double y)
//{
//    TangentWidget *tangent;
//    TangentPoints tangentPoints;

//    bool found = false;

//    for(int i = 0 ; i < tangents->size() && !found; i++)
//    {
//        tangent = tangents->at(i);

//        if(!tangent->isTangentValid())
//            continue;

//        tangentPoints = tangent->getCaracteristicPoints();

//        if(fabs(tangentPoints.left.x() - x) < 4/pxPerUnit.x && fabs(tangentPoints.left.y() - y) < 4/pxPerUnit.y)
//        {
//            mouseState.tangentHovering = true;
//            mouseState.tangentPtSelection = -1; //-1 for left point, so when we add dx to the tangent's lenght, we multiply it by -1
//            mouseState.pointedObjectType = TANGENT_RESIZE;
//            mouseState.id = i;
//            found = true;
//            //recalculate = false;
//        }
//        else if(fabs(tangentPoints.right.x() - x) < 4/pxPerUnit.x && fabs(tangentPoints.right.y() - y) < 4/pxPerUnit.y)
//        {
//            mouseState.tangentHovering = true;
//            mouseState.tangentPtSelection = 1;
//            mouseState.pointedObjectType = TANGENT_RESIZE;
//            mouseState.id = i;
//            found = true;
//            //recalculate = false;
//        }
//        else if(fabs(tangentPoints.center.x() - x) < 5/pxPerUnit.x && fabs(tangentPoints.center.y() - y) < 5/pxPerUnit.y)
//        {
//            mouseState.tangentHovering = true;
//            mouseState.pointedObjectType = TANGENT_MOVE;
//            mouseState.id = i;
//            found = true;
//            //recalculate = false;
//        }
//    }

//    if(found)
//        update();
//}

//void SubGraph::drawTicksAndNumbers()
//{
//    pen.setColor(parameters.axesColor);
//    pen.setWidth(1);
//    painter.setPen(pen);
//    painter.setRenderHint(QPainter::Antialiasing, false);

//    double start, end, Ypos, posTxt;

//    //trace sur l'axe des X
//    if(centre.y < 20)
//    {
//        Ypos = 20;
//        posTxt = Ypos + parameters.graphFont.pixelSize() + 3;
//    }
//    else if(graphHeight - centre.y < 20)
//    {
//        Ypos = graphHeight - 20;
//        posTxt = Ypos - 7;
//    }
//    else
//    {
//        Ypos = centre.y;
//        posTxt = Ypos + parameters.graphFont.pixelSize() + 3;
//    }

//    double Xreal = trunc(graphRange.Xmin / graphRange.Xstep) * graphRange.Xstep;
//    double Xpos = Xreal * pxPerUnit.x + centre.x;
//    double pos;

//    double step = graphRange.Xstep * pxPerUnit.x;

//    double bas = height();
//    double haut = 0;

//    QString num = QString::number(Xreal, 'g', NUM_PREC);
//    widestXNumber = painter.fontMetrics().width(num);

//    start = 5;
//    end = graphWidth - 5;

//    if(centre.x < 10)
//        start = 10 + painter.fontMetrics().width(num)/2 + 5;
//    else if(centre.x > graphWidth - 10)
//        end = graphWidth - 10 - painter.fontMetrics().width(num)/2 - 5;

//    while(Xpos <= end)
//    {
//        if(start <= Xpos && fabs(Xpos - centre.x) > 1)
//        {
//            if(start <= Xpos && information.getGridState() && Xpos <= end)
//            {
//                pen.setColor(parameters.gridColor);
//                pen.setWidthF(0.5);
//                painter.setPen(pen);
//                painter.drawLine(QPointF(Xpos, bas), QPointF(Xpos, haut));

//                pen.setColor(parameters.axesColor);
//                pen.setWidth(1);
//                painter.setPen(pen);
//            }

//            painter.drawLine(QPointF(Xpos, Ypos -3), QPointF(Xpos, Ypos));
//            num = QString::number(Xreal, 'g', NUM_PREC);
//            pos = Xpos - painter.fontMetrics().width(num)/2;
//            painter.drawText(QPointF(pos, posTxt), num);

//            if(painter.fontMetrics().width(num) > widestXNumber)
//                widestXNumber = painter.fontMetrics().width(num);
//        }

//        Xpos += step;
//        Xreal += graphRange.Xstep;
//    }

////trace sur l'axe des Y

//    bool drawOnRight = false;

//    if(centre.x < 10)
//    {
//        Xpos = 10;
//        posTxt = Xpos + 4;
//    }
//    else if(graphWidth - centre.x < 10)
//    {
//        Xpos = graphWidth - 10;
//        posTxt = Xpos - 8;
//        drawOnRight = true;
//    }
//    else
//    {
//        Xpos = centre.x;
//        posTxt = Xpos + 5;
//    }

//    double Yreal = trunc(graphRange.Ymax / graphRange.Ystep) * graphRange.Ystep;
//    Ypos = -Yreal * pxPerUnit.y + centre.y;
//    step = graphRange.Ystep * pxPerUnit.y;

//    bas =  0;
//    haut =  graphWidth;

//    start = 5;
//    end = graphHeight - 5;


//    if(graphHeight - centre.y < 10)
//        end = graphHeight - 50;
//    else if(centre.y < 10)
//        start = 50;


//    double txtCorr = + painter.fontMetrics().ascent()/2 - 2;

//    while(Ypos <= end)
//    {
//        if(start <= Ypos && fabs(Ypos - centre.y) > 1)
//        {
//            if(information.getGridState())
//            {
//                pen.setColor(parameters.gridColor);
//                pen.setWidthF(0.5);
//                painter.setPen(pen);
//                painter.drawLine(QPointF(bas, Ypos), QPointF(haut, Ypos));

//                pen.setColor(parameters.axesColor);
//                pen.setWidth(1);
//                painter.setPen(pen);
//            }

//            painter.drawLine(QPointF(Xpos  -3, Ypos), QPointF(Xpos, Ypos));
//            num = QString::number(Yreal, 'g', NUM_PREC);
//            if(drawOnRight)
//                painter.drawText(QPointF(posTxt - painter.fontMetrics().width(num), Ypos + txtCorr), num);
//            else painter.drawText(QPointF(posTxt, Ypos + txtCorr), num);
//        }

//        Yreal -= graphRange.Ystep;
//        Ypos += step;
//    }
//}

//void SubGraph::incrementTickSpacing(double &spacing, int &currentMultiplier)
//{
//    switch (currentMultiplier) {
//    case 1:
//        currentMultiplier = 2;
//        spacing *= 2;
//        break;
//    case 2:
//        currentMultiplier = 5;
//        spacing *= 2.5;
//        break;
//    case 5:
//        currentMultiplier = 1;
//        spacing *= 2;
//        break;
//    }
//}

//void SubGraph::decrementTickSpacing(double &spacing, int &currentMultiplier)
//{
//    switch (currentMultiplier) {
//    case 1:
//        currentMultiplier = 5;
//        spacing /= 2;
//        break;
//    case 5:
//        currentMultiplier = 2;
//        spacing /= 2.5;
//        break;
//    case 2:
//        currentMultiplier = 1;
//        spacing /= 2;
//        break;
//    }
//}

//void SubGraph::drawAxes()
//{
//    // *********** remarque: les y sont positifs en dessous de l'axe x, step au dessus !! ************//
//    pen.setWidth(1);
//    pen.setColor(parameters.axesColor);
//    painter.setPen(pen);
//    painter.setRenderHint(QPainter::Antialiasing, false);

//    axesIntersec.y = centre.y;
//    axesIntersec.x = centre.x;

//    if(graphRange.Ymin > -20/pxPerUnit.y)
//        axesIntersec.y = graphHeight - 20;
//    else if(graphRange.Ymax < 20/pxPerUnit.y)
//        axesIntersec.y = 20;

//    if(graphRange.Xmin > -10/pxPerUnit.x)
//        axesIntersec.x = 10;
//    else if(graphRange.Xmax < 10/pxPerUnit.x)
//        axesIntersec.x = graphWidth - 10;


//    //ordinates axis
//    painter.drawLine(QPointF(axesIntersec.x, 0), QPointF(axesIntersec.x, graphHeight));

//    //abscissa axis
//    painter.drawLine(QPointF(0, axesIntersec.y), QPointF(graphWidth, axesIntersec.y));


//    if(graphRange.Ymin > -20/pxPerUnit.y)
//    {
//        painter.drawLine(QPointF(axesIntersec.x-3, axesIntersec.y-6), QPointF(axesIntersec.x+3, axesIntersec.y-4));
//        painter.drawLine(QPointF(axesIntersec.x-3, axesIntersec.y-9), QPointF(axesIntersec.x+3, axesIntersec.y-7));

//        pen.setColor(parameters.backgroundColor);
//        painter.setPen(pen);

//        painter.drawLine(QPointF(axesIntersec.x, axesIntersec.y-6), QPointF(axesIntersec.x, axesIntersec.y-7));
//    }
//    else if(graphRange.Ymax < 20/pxPerUnit.y)
//    {
//        painter.drawLine(QPointF(axesIntersec.x-3, axesIntersec.y+6), QPointF(axesIntersec.x+3, axesIntersec.y+4));
//        painter.drawLine(QPointF(axesIntersec.x-3, axesIntersec.y+9), QPointF(axesIntersec.x+3, axesIntersec.y+7));

//        pen.setColor(parameters.backgroundColor);
//        painter.setPen(pen);

//        painter.drawLine(QPointF(axesIntersec.x, axesIntersec.y+6), QPointF(axesIntersec.x, axesIntersec.y+7));
//    }
//}

//void SubGraph::zoomX()
//{
//    double valeur = (graphRange.Xmax- graphRange.Xmin) * double(hSlider->value()) * 0.0016;

//    double ratio = (viewMapper.getViewRect().right()- viewMapper.getViewRect().left()) * (double)(hSlider->value()) * 0.0016;
//    /* TODO: Put that in the graphView code
//      if( (win.Xmax - win.Xmin > MIN_AMPLITUDE and win.Ymax - win.Ymin > MIN_AMPLITUDE) and
//            (win.Xmax - win.Xmin < MAX_AMPLITUDE and win.Ymax - win.Ymin < MAX_AMPLITUDE))
//    */
//    viewMapper.zoomXview(ratio);
//    moving = true;
//    information.setRange(viewMapper);

//    if(graphSettings.smoothing)
//        repaintTimer.start();
//}

//void SubGraph::stop_X_zoom()
//{
//    timerX.stop();
//    hSlider->setValue(0);
//}

//void SubGraph::zoomY()
//{
//    double valeur = (graphRange.Ymax - graphRange.Ymin) * double(vSlider->value()) * 0.0016;

//    double valeur = (viewMapper.getViewRect().top() - viewMapper.getViewRect().bottom()) * (double)(vSlider->value()) * 0.0016;

//    if(!information.isOrthonormal())
//    {
//        viewMapper.zoomYview(valeur);
//        recalculate = false;
//    }
//    else
//    {
//        viewMapper.zoomXview(valeur);
//        recalculate = true;
//    }

//    moving = true;
//    if(graphSettings.smoothing)
//        repaintTimer.start();

//    information.setRange(viewMapper);
//}

//void SubGraph::stop_Y_Zoom()
//{
//    timerY.stop();
//    vSlider->setValue(0);
//}

//SubGraph::~SubGraph()
//{
//    delete savedGraph;
//    delete exprCalculator;
//}
