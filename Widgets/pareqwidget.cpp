/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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




#include "Widgets/pareqwidget.h"

ParEqWidget::ParEqWidget(int num, QList<FuncCalculator*> list, QColor col) : treeCreator(ObjectType::PARAMETRIC_EQ), colorSaver(col)
{
    calculator = new ExprCalculator(true, list);
    index = num;
    funcCalcs = list;
    createWidgets(col);
    xTree = yTree = NULL;   
    isParametric = valid = is_t_range_parametric = false;
    playState = false;
    blockAnimation = false;
    increment = 1;
    current_pos = 1;
    current_t = current_k = 0;

    QColor color;
    color.setNamedColor(VALID_COLOR);
     validPalette.setColor(QPalette::Base, color);
    validPalette.setColor(QPalette::Text, Qt::black);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);
    invalidPalette.setColor(QPalette::Text, Qt::black);

    connect(xLine, SIGNAL(textChanged(QString)), this, SLOT(checkExpr()));
    connect(yLine, SIGNAL(textChanged(QString)), this, SLOT(checkExpr()));
}

void ParEqWidget::createWidgets(QColor col)
{
    mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(3);
    mainLayout->setMargin(5);

    addExprWidgets(col);
    addTConfWidgets();
    addKConfWidgets();
    addAnimationControllWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    mainLayout->addWidget(frame);

    setLayout(mainLayout);
}

bool ParEqWidget::isParEqParametric()
{
    return isParametric;
}

void ParEqWidget::recalculatePointsList()
{
    if(valid)
        calculatePointsList();
}

void ParEqWidget::setRatio(double r)
{    
    ratio = r;

    if(tWidget->isAnimateChecked() && valid)
    {
        if(2*trunc(tRange.step * ratio) > trunc(tRange.end - tRange.start))
        {
            blockAnimation = true;
            parCurrentValLineEdit->setText(tr("The increment period is too small..."));
        }
        else
        {
            blockAnimation = false;
            calculatePointsList();
            current_tPos = (current_t - tRange.start) / (tRange.step * ratio);
            current_t = tRange.start + (double)current_tPos * tRange.step * ratio;
            updateAnimationSlider();
        }
    }
    else if(kWidget->isAnimateChecked() && valid)
    {         
        if(2*trunc(kRange.step * ratio) > trunc(kRange.end - kRange.start))
        {
            blockAnimation = true;
            parCurrentValLineEdit->setText(tr("The increment period is too small..."));
        }
        else
        {
            blockAnimation = false;
            current_kPos = trunc((current_k - kRange.start) / (kRange.step * ratio));
            current_k = kRange.step + (double)current_kPos * kRange.step * ratio;
            curvesNum_current = trunc((kRange.end - kRange.start) / (kRange.step * ratio)) + 1;
            colorSaver.setCurvesNum(curvesNum_current);
            updateAnimationSlider();
        }
    }
}

void ParEqWidget::changeID(int newID)
{
    index = newID;
    parEqNameLabel = new QLabel("(P<sub>" + QString::number(index + 1) + "</sub>)");
}

void ParEqWidget::addExprWidgets(QColor col)
{
    QHBoxLayout *layout1 = new QHBoxLayout();    

    QVBoxLayout *vLayout = new QVBoxLayout;

    parEqNameLabel = new QLabel("(P<sub>" + QString::number(index + 1) + "</sub>)");
    parEqNameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    drawCheckBox = new QCheckBox;
    drawCheckBox->setChecked(true);
    drawCheckBox->setMinimumSize(20,20);
    connect(drawCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(updateRequest()));

    vLayout->addWidget(parEqNameLabel);
    vLayout->addWidget(drawCheckBox);

    layout1->addLayout(vLayout);

    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::VLine);


    layout1->addWidget(frame);

    QVBoxLayout *eqLayout = new QVBoxLayout();
    eqLayout->setSpacing(3);

    QHBoxLayout *xLayout = new QHBoxLayout();

    QLabel *xLabel = new QLabel("x(t) =");
    xLine = new QLineEdit();
    xLine->setMaximumHeight(25);
    connect(xLine, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    xLayout->addWidget(xLabel);
    xLayout->addWidget(xLine);

    QHBoxLayout *yLayout = new QHBoxLayout();

    QLabel *yLabel = new QLabel("y(t) =");

    yLine = new QLineEdit();
    yLine->setMaximumHeight(25);
    connect(yLine, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    yLayout->addWidget(yLabel);
    yLayout->addWidget(yLine);

    eqLayout->addLayout(xLayout);
    eqLayout->addLayout(yLayout);

    layout1->addLayout(eqLayout);

    QVBoxLayout *buttonsLayout = new QVBoxLayout();

    QHBoxLayout *colorButtonsLayout = new QHBoxLayout;
    colorButtonsLayout->setMargin(5);
    colorButtonsLayout->setSpacing(3);

    QColorButton *firstColorButton = new QColorButton(col);
    lastColorButton = new QColorButton(col);

    connect(firstColorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setFristColor(QColor)));
    connect(lastColorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setLastColor(QColor)));
    connect(&colorSaver, SIGNAL(colorsChanged()), this, SIGNAL(updateRequest()));

    colorButtonsLayout->addWidget(firstColorButton);
    colorButtonsLayout->addWidget(lastColorButton);

    lastColorButton->hide();

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setFlat(true);
    removeButton->setIconSize(QSize(25,25));
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveSignal()));

    buttonsLayout->addLayout(colorButtonsLayout);
    buttonsLayout->addWidget(removeButton);

    layout1->addLayout(buttonsLayout);

    mainLayout->addLayout(layout1);
}

void ParEqWidget::addTConfWidgets()
{
    tWidget = new ParConfWidget('t', true, true);
    tWidget->setFuncsList(funcCalcs); 

    connect(tWidget, SIGNAL(k_typed(bool)), this, SLOT(set_tRange_parametric(bool)));
    connect(tWidget, SIGNAL(animateToggled(bool)), this, SLOT(animateTtoggled(bool)));
    connect(tWidget, SIGNAL(animateToggled(bool)), this, SIGNAL(updateRequest()));
    connect(tWidget, SIGNAL(animateToggled(bool)), this, SLOT(recalculatePointsList()));
    connect(tWidget, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    mainLayout->addSpacing(15);
    mainLayout->addWidget(tWidget);

}

 void ParEqWidget::set_tRange_parametric(bool parametric)
 {
     is_t_range_parametric = parametric;
 }

 Range ParEqWidget::getKRange()
 {
     return kRange;
 }

 Range ParEqWidget::getTRange(double k)
 {
     return tWidget->getRange(k);
 }

 ParEqValues ParEqWidget::getParEqValues(Range t_range, double k)
 {
     ParEqValues vals;

     int end = trunc((t_range.end - t_range.start)/t_range.step) + 1;
     double t = t_range.start;

     calculator->setK(k);

     for(int i = 0 ; i < end ; i++)
     {
         vals.tValues << t;
         vals.xValues << calculator->calculateFromTree(xTree, t);
         vals.yValues << calculator->calculateFromTree(yTree, t);

         t += t_range.step;
     }

     return vals;
 }

 Point ParEqWidget::getPoint(double t, double k)
 {
     Point pt;

     calculator->setK(k);
     pt.x = calculator->calculateFromTree(xTree, t);
     pt.y = calculator->calculateFromTree(yTree, t);

     return pt;
 }

void ParEqWidget::addKConfWidgets()
{
    kWidget = new ParConfWidget('k', true);
    kWidget->setFuncsList(funcCalcs);

    connect(kWidget, SIGNAL(animateToggled(bool)), this, SLOT(animateKtoggled(bool)));
    connect(kWidget, SIGNAL(animateToggled(bool)), this, SIGNAL(updateRequest()));
    connect(kWidget, SIGNAL(animateToggled(bool)), this, SLOT(recalculatePointsList()));
    connect(kWidget, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    mainLayout->addWidget(kWidget);
    mainLayout->addSpacing(15);

    kWidget->hide();
}

void ParEqWidget::addAnimationControllWidgets()
{
    animationControlWidget = new QWidget;

    QGridLayout *animCtrLayout = new QGridLayout;
    animCtrLayout->setMargin(2);

    play = new QPushButton;
    play->setFixedSize(25,25);
    play->setIconSize(QSize(18,18));
    play->setIcon(QIcon(":/icons/play.png"));

    connect(play, SIGNAL(released()), this, SLOT(playButtonClicked()));

    loopFromStart = new QPushButton;
    loopFromStart->setCheckable(true);    
    loopFromStart->setFixedSize(25,25);
    loopFromStart->setIconSize(QSize(18,18));
    loopFromStart->setIcon(QIcon(":/icons/loop.png"));

    loopRound = new QPushButton;
    loopRound->setCheckable(true);        
    loopRound->setFixedSize(25,25);
    loopRound->setIconSize(QSize(18,18));
    loopRound->setIcon(QIcon(":/icons/go&return.png"));

    connect(loopFromStart, SIGNAL(toggled(bool)), this, SLOT(loopFromStartToggled()));
    connect(loopRound, SIGNAL(toggled(bool)), this, SLOT(loopRoundToggled()));       

    parSlider = new QSlider(Qt::Horizontal);
    connect(parSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));    

    parCharLabel = new QLabel();
    parCurrentValLineEdit = new QLineEdit();
    parCurrentValLineEdit->setFrame(false);
    parCurrentValLineEdit->setReadOnly(true);
    parCurrentValLineEdit->setMaximumHeight(25);    

    animCtrLayout->addWidget(play, 0, 0, 1, 1);
    animCtrLayout->addWidget(loopFromStart, 0, 1, 1, 1);
    animCtrLayout->addWidget(loopRound, 0, 2, 1, 1);
    animCtrLayout->addWidget(parSlider, 0, 3, 1, 3);
    animCtrLayout->addWidget(parCharLabel, 1, 4, 1, 1);
    animCtrLayout->addWidget(parCurrentValLineEdit, 1, 5, 1, 1);

    animationControlWidget->setLayout(animCtrLayout);

    mainLayout->addWidget(animationControlWidget);

    animationControlWidget->hide();
}

void ParEqWidget::loopFromStartToggled()
{
    if(loopFromStart->isChecked() && loopRound->isChecked())
        loopRound->setChecked(false);
}

void ParEqWidget::loopRoundToggled()
{
    if(loopFromStart->isChecked() && loopRound->isChecked())
        loopFromStart->setChecked(false);
}

void ParEqWidget::sliderMoved(int pos)
{
    if(isAnimated())
    {
        if(is_t_Animated())
            current_tPos = pos - increment;
        else current_kPos = pos - increment;

        nextFrame();
        emit animationUpdateRequest();
    }
}

void ParEqWidget::animateKtoggled(bool checked)
{    
    tWidget->setAnimationEnabled(!checked);
    animationControlWidget->setHidden(!checked);
    playState = false;
    play->setIcon(QIcon(":/icons/play.png"));

    if(checked)
    {
        parCharLabel->setText("k =");
        setRatio(ratio);
    }
    else
    {
        curvesNum_current = curvesNum_original;
    }

    colorSaver.setCurvesNum(curvesNum_current);

    updateAnimationSlider();

}

void ParEqWidget::animateTtoggled(bool checked)
{   
    kWidget->setAnimationEnabled(!checked);
    animationControlWidget->setHidden(!checked);
    playState = false;
    play->setIcon(QIcon(":/icons/play.png"));
    updateAnimationSlider();

    if(checked)
    {
        parCharLabel->setText("t =");
        setRatio(ratio);
    }
}

bool ParEqWidget::keepTracks()
{
    return tWidget->doesKeepTracks();
}

void ParEqWidget::checkExpr()
{
    are_expr_parametric = xLine->text().contains('k') || yLine->text().contains('k');
    isParametric = are_expr_parametric || is_t_range_parametric;
    kWidget->setHidden(!isParametric);
    lastColorButton->setHidden(!isParametric);

    if(!isParametric && kWidget->isAnimateChecked())
        kWidget->setAnimationChecked(false);
}

void ParEqWidget::apply()
{
    hasSomethingChanged = false;

    updateKRange();
    checkXline();
    checkYline();

    tWidget->validate();
    updateTRange(kRange.start);

    valid = isXExprGood && isYExprGood && isTRangeGood && isKRangeGood;

    if(hasSomethingChanged && valid)
    {
        calculatePointsList();

        currentPolygon.clear();

        if(tWidget->isAnimateChecked())
            for(short i = 0 ; i < pointsList.size(); i++)
                currentPolygon << pointsList[i].mid(0, 1);

        else if(kWidget->isAnimateChecked())
            currentPolygon << pointsList[0];

        current_tPos = 0;
        current_kPos = 0;
        increment = 1;
        current_t = tRange.start;
        current_k = kRange.start;

        if(tWidget->isAnimateChecked() || kWidget->isAnimateChecked())
        {
             updateAnimationSlider();
             parSlider->setValue(0);
        }
    }
}

void ParEqWidget::checkXline()
{
    if(xExpr != xLine->text())
    {
        if(xTree != NULL)
            treeCreator.deleteFastTree(xTree);

        xTree = treeCreator.getTreeFromExpr(xLine->text(), isXExprGood);

        if(isXExprGood)
            xLine->setPalette(validPalette);
        else xLine->setPalette(invalidPalette);

        xExpr = xLine->text();
        hasSomethingChanged = true;
    }
}

void ParEqWidget::checkYline()
{
    if(yExpr != yLine->text())
    {
        if(yTree != NULL)
            treeCreator.deleteFastTree(yTree);

        yTree = treeCreator.getTreeFromExpr(yLine->text(), isYExprGood);

        if(isYExprGood)
            yLine->setPalette(validPalette);
        else yLine->setPalette(invalidPalette);

        yExpr = yLine->text();
        hasSomethingChanged = true;
    }
}

void ParEqWidget::updateTRange(double k)
{    
    if(!areIdentical(tRange, tWidget->getRange(k)))
        hasSomethingChanged = true;

    tRange = tWidget->getRange(k);
    tPos_end = trunc((tRange.end - tRange.start)/(tRange.step * ratio)) + 1;

    int pointsNum = trunc((tRange.end - tRange.start)/tRange.step) + 1;

    isTRangeGood = tWidget->isValid() && 20000 > pointsNum && pointsNum > 0;
    if(!isTRangeGood)
        valid = false;

    if(pointsNum  <= 0)
        QMessageBox::warning(this, tr("Error"),tr("Step value for the \"t\" parameter is not compatible with the entered range, in parametric equation") + " (P<sub>" + QString::number(index) + "</sub>).");

    else if(pointsNum  > 20000)
        QMessageBox::warning(this, tr("Error"), tr("Too many points to calculate on parametric equation") + " (P<sub>" + QString::number(index) + "</sub>).");

}

void ParEqWidget::updateKRange()
{
    kWidget->validate();

    if(!areIdentical(kRange, kWidget->getRange()))
        hasSomethingChanged = true;

    kRange = kWidget->getRange();

    curvesNum_original = curvesNum_current = trunc((kRange.end - kRange.start)/kRange.step) + 1;
    if(kWidget->isAnimateChecked())
        curvesNum_current = trunc((kRange.end - kRange.start)/(kRange.step*ratio)) + 1;
    else curvesNum_current = curvesNum_original;

    colorSaver.setCurvesNum(curvesNum_current);

    isKRangeGood = kWidget->isValid() && curvesNum_original > 0;

    if(curvesNum_original <= 0)
        QMessageBox::warning(this, tr("Error"),
                             tr("Step value for the \"k\" parameter is not compatible with the entered range, in parametric equation (P<sub>")
                             + QString::number(index) + "</sub>).");
}

ColorSaver* ParEqWidget::getColorSaver()
{
    return &colorSaver;
}

void ParEqWidget::calculatePointsList()
{
    int numDraws = 1, end;
    double k = kRange.start, t = 0, tStep = 0, tRatio = 1;

    if(tWidget->isAnimateChecked() && ratio < 1)
        tRatio = ratio;

    if(!kWidget->isAnimateChecked())
        numDraws = curvesNum_original;

    pointsList.clear();

    Point point;

    for(int draw = 0; draw < numDraws && draw < PAR_DRAW_LIMIT; draw++)
    {
        QList<Point> list;
        calculator->setK(k);
        updateTRange(k);

        if(!isTRangeGood)
            return;

        tStep = tRange.step * tRatio;
        end = trunc((tRange.end - tRange.start)/tStep) + 1;
        t = tRange.start;

        for(int i = 0 ; i < end ; i++)
        {
            point.x = calculator->calculateFromTree(xTree, t);
            point.y = calculator->calculateFromTree(yTree, t);

            list << point;

            t += tStep;
        }

        pointsList << list;

        k += kRange.step;
    }
}

void ParEqWidget::updateAnimationSlider()
{
    if(tWidget->isAnimateChecked())
    {
        parCurrentValLineEdit->setText(QString::number(current_t, 'g', NUM_PREC));
        parSlider->setMinimum(0);
        parSlider->setMaximum(tPos_end);
    }
    else if(kWidget->isAnimateChecked())
    {
        parCurrentValLineEdit->setText(QString::number(current_k, 'g', NUM_PREC));
        parSlider->setMinimum(0);
        parSlider->setMaximum(curvesNum_current - 1);
    }
}

bool ParEqWidget::isPlaying()
{
    return playState;
}

bool ParEqWidget::isValid()
{
    return valid;
}

bool ParEqWidget::getDrawState()
{
    return valid && drawCheckBox->isChecked();
}

void ParEqWidget::nextFrame()
{    
    if(tWidget->isAnimateChecked())
        nextFrameTchecked();
    else if(kWidget->isAnimateChecked())
        nextFrameKchecked();
}

void ParEqWidget::nextFrameTchecked()
{
    if((current_tPos == 0 && increment < 0) || (current_tPos == tPos_end && increment > 0))
    {
        if(loopRound->isChecked())
        {
            increment = - increment;
        }
        else if(loopFromStart->isChecked())
        {
            current_tPos = 0;
            current_t = tRange.start;
            increment = 1;
        }
        else
        {
            play->setIcon(QIcon(":/icons/play.png"));
            playState = false;
            increment = 1;
            return;
        }
    }

    current_tPos += increment;
    current_pos = current_tPos * (pointsList[0].size() - 1) / tPos_end + 1;

    current_t = tRange.start + (double)current_tPos * tRange.step * ratio;
    parCurrentValLineEdit->setText(QString::number(current_t, 'g', NUM_PREC));
    parSlider->setValue(current_tPos);
}

void ParEqWidget::nextFrameKchecked()
{
    if((current_kPos <= 0 && increment < 0) || (current_kPos >= curvesNum_current - 1 && increment > 0))
    {
        if(loopRound->isChecked())
        {
            increment = - increment;
        }
        else if(loopFromStart->isChecked())
        {
            current_kPos = 0;
            current_k = kRange.step;
            increment = 1;
        }
        else
        {
            play->setIcon(QIcon(":/icons/play.png"));
            playState = false;
            increment = 1;
            return;
        }
    }

    current_kPos += increment;
    current_k = kRange.start + (double)(current_kPos) * kRange.step * ratio;

    updateTRange(current_k);
    if(!isTRangeGood)
        return;

    currentPolygon.clear();
    currentPolygon << QList<Point>();


    calculator->setK(current_k);

    double t = tRange.start;
    int end = trunc((tRange.end - tRange.start)/tRange.step)+ 1;
    Point point;

    for(int i = 0 ; i < end ; i++)
    {
        point.x = calculator->calculateFromTree(xTree, t);
        point.y = calculator->calculateFromTree(yTree, t);

        currentPolygon[0] << point;

        t += tRange.step;
    }

    parCurrentValLineEdit->setText(QString::number(current_k, 'g', NUM_PREC));
    parSlider->setValue(current_kPos);
}

int ParEqWidget::getCurrentKPos()
{
    return current_kPos;
}

int ParEqWidget::getCurrentTPos()
{
    return current_pos;
}

bool ParEqWidget::is_t_Animated()
{
    return tWidget->isAnimateChecked();
}

bool ParEqWidget::isAnimated()
{
    return (kWidget->isAnimateChecked() || tWidget->isAnimateChecked())  && valid && !blockAnimation;
}

QList< QList<Point> >* ParEqWidget::getCurrentPolygon()
{
    return &currentPolygon;
}

QList< QList<Point> >* ParEqWidget::getPointsList()
{
    return &pointsList;
}

bool ParEqWidget::areIdentical(Range a, Range b)
{
    return a.start == b.start && a.end == b.end && a.step == b.step;
}

void ParEqWidget::emitRemoveSignal()
{
    emit removeClicked(this);
}

void ParEqWidget::playButtonClicked()
{
    if(playState)
    {       
        play->setIcon(QIcon(":/icons/play.png"));       
    }
    else
    {       
        play->setIcon(QIcon(":/icons/pause.png"));       
    }

    playState = !playState;
}

ParEqWidget::~ParEqWidget()
{
    if(xTree != NULL)
        treeCreator.deleteFastTree(xTree);
    if(yTree != NULL)
        treeCreator.deleteFastTree(yTree);
}

