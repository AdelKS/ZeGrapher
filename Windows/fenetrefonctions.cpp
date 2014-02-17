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


#include "Windows/fenetrefonctions.h"
#include "ui_fenetrefonctions.h"


FenetreFonctions::FenetreFonctions(Informations *info)
{   
    ui = new Ui::FenetreFonctions;
    ui->setupUi(this);   

    parEqController = new ParEqController(&parEqWidgets);

    connect(ui->periodSpinBox, SIGNAL(valueChanged(int)), parEqController, SLOT(setIncrPeriod(int)));
    connect(ui->freqSpinBox, SIGNAL(valueChanged(int)), parEqController, SLOT(setUpdateFreq(int)));

    connect(parEqController, SIGNAL(animationUpdate()), info, SLOT(emitAnimationUpdate()));

    setWindowTitle(tr("Tracer"));
    setWindowIcon(QIcon(":/icons/fonctions.png"));

    funcNames << 'f' << 'g' << 'h'<< 'p' << 'r'<< 'm';
    seqNames << 'u' << 'v' << 'l' << 'w' << 'q' << 't';

    setInfoClass(info);//must be before addFunctions and addSequences, because they use informations class
    addFunctions();
    addSequences();

    ui->periodSpinBox->setValue(INIT_INCR_PERIOD);
    ui->freqSpinBox->setValue(INIT_FREQ);



    connect(ui->boutonTracer, SIGNAL(released()), this, SLOT(draw()));
    connect(ui->addLine, SIGNAL(released()), this, SLOT(addStraightline()));
    connect(ui->addTangent, SIGNAL(released()), this, SLOT(addTangent()));
    connect(ui->addParEq, SIGNAL(released()), this, SLOT(addParEq()));
    connect(ui->addDataWidget, SIGNAL(released()), this, SLOT(addDataWidget()));

    connect(ui->keyboardButton, SIGNAL(released()), this, SLOT(keyboardButtonClicked()));
}

void FenetreFonctions::addFunctions()
{   
    for(int i = 0 ; i < funcNames.size() ; i++)
    {
        FuncWidget *widget = new FuncWidget(funcNames[i], i, Qt::black, window());
        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), informations, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState(int)), this, SLOT(newFuncParametricState()));

        ui->funcWidgetsLayout->addWidget(widget);

        funcCalcs << widget->getCalculator();
        funcWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->funcWidgetsLayout->addWidget(frame);
    }

    informations->setFunctionsList(funcCalcs);

    for(int i = 0 ; i < funcCalcs.size() ; i++)
    {
        funcWidgets[i]->setFuncWidgets(funcWidgets);
        funcWidgets[i]->setFuncsCalcsList(funcCalcs);
        funcCalcs[i]->setFuncsPointers(funcCalcs);
    }
}

void FenetreFonctions::addSequences()
{
    for(int i = 0 ; i < seqNames.size() ; i++)
    {
        SeqWidget *widget = new SeqWidget(seqNames[i], i, Qt::black);
        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), informations, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState()), this, SLOT(newSeqParametricState()));

        ui->seqWidgetsLayout->addWidget(widget);

        seqCalcs << widget->getCalculator();
        seqWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->seqWidgetsLayout->addWidget(frame);
    }

    informations->setSequencesList(seqCalcs);

    for(int i = 0 ; i < seqCalcs.size() ; i++)
    {
        connect(ui->nMin, SIGNAL(valueChanged(int)), seqCalcs[i], SLOT(set_nMin(int)));
        seqCalcs[i]->setFuncsPointers(funcCalcs);
        seqCalcs[i]->setSeqsPointers(seqCalcs);
        seqWidgets[i]->setFuncsList(funcCalcs);
        seqWidgets[i]->setFuncWidgets(funcWidgets);
        seqWidgets[i]->setSeqWidgets(seqWidgets);
    }
}

void FenetreFonctions::newFuncParametricState()
{
    for(int i = 0 ; i < funcWidgets.size() ; i++)
        funcWidgets[i]->checkCalledFuncsParametric();

    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void FenetreFonctions::newSeqParametricState()
{
    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void FenetreFonctions::setInfoClass(Informations *info)
{
    informations = info;   
    informations->setParEqsListPointer(&parEqWidgets);
    informations->setTangentsListPointer(&tangentWidgets);
    informations->setStraightLinesListPointer(&straightlineWidgets);   
}

void FenetreFonctions::draw()
{    
    validateFunctions();
    validateSequences();
    validateLines();
    validateParametricEquations();
    informations->emitUpdateSignal();
}

void FenetreFonctions::validateFunctions()
{
    for(int i = 0 ; i < funcWidgets.size(); i++)
        funcWidgets[i]->firstValidation();

    for(int i = 0 ; i < funcWidgets.size(); i++)
        funcWidgets[i]->secondValidation();
}

void FenetreFonctions::validateSequences()
{
    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->firstValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->secondValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->thirdValidation();
}

void FenetreFonctions::validateLines()
{
    for(int i = 0 ; i < straightlineWidgets.size(); i++)
        straightlineWidgets[i]->validate();

    for(int i = 0 ; i < tangentWidgets.size(); i++)
        tangentWidgets[i]->validate();

}

void FenetreFonctions::validateParametricEquations()
{
    for(int i = 0 ; i < parEqWidgets.size(); i++)
        parEqWidgets[i]->apply();
}

void FenetreFonctions::keyboardButtonClicked()
{
    emit displayKeyboard();
}

void FenetreFonctions::addTangent()
{
    TangentWidget *tangent = new TangentWidget(tangentWidgets.size(), funcCalcs, funcWidgets);
    tangentWidgets << tangent;

    connect(tangent, SIGNAL(removeMe(TangentWidget*)), this, SLOT(removeTangent(TangentWidget*)));
    connect(tangent, SIGNAL(returnPressed()), this, SLOT(draw()));
    connect(tangent, SIGNAL(drawStateChanged()), informations, SLOT(emitDrawStateUpdate()));

    ui->linesLayout->addWidget(tangent);
}

void FenetreFonctions::removeTangent(TangentWidget *widget)
{    
    for(int i = tangentWidgets.indexOf(widget) + 1 ; i < tangentWidgets.size() ; i++)
        tangentWidgets[i]->changeID(i-1);

    tangentWidgets.removeOne(widget);
    widget->close();
    delete widget;

    informations->emitUpdateSignal();
}

void FenetreFonctions::addStraightline()
{
    StraightLineWidget *line = new StraightLineWidget(straightlineWidgets.size(), funcCalcs);
    straightlineWidgets << line;

    connect(line, SIGNAL(removeMe(StraightLineWidget*)), this, SLOT(removeStraightline(StraightLineWidget*)));
    connect(line, SIGNAL(returnPressed()), this, SLOT(draw()));
    connect(line, SIGNAL(drawStateChanged()), informations, SLOT(emitDrawStateUpdate()));

    ui->linesLayout->addWidget(line);
}

void FenetreFonctions::removeStraightline(StraightLineWidget *widget)
{
    for(int i = straightlineWidgets.indexOf(widget) + 1; i < straightlineWidgets.size(); i++)
        straightlineWidgets[i]->changeID(i-1);

    straightlineWidgets.removeOne(widget);
    widget->close();
    delete widget;

}

void FenetreFonctions::addParEq()
{
    ParEqWidget *widget = new ParEqWidget(parEqWidgets.size(), funcCalcs);
    connect(widget, SIGNAL(removeClicked(ParEqWidget*)), this, SLOT(removeParEq(ParEqWidget*)));
    connect(widget, SIGNAL(updateRequest()), informations, SLOT(emitDrawStateUpdate()));
    connect(widget, SIGNAL(animationUpdateRequest()), informations, SLOT(emitAnimationUpdate()));
    connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));

    parEqWidgets << widget;
    ui->parEqLayout->addWidget(widget);

    parEqController->newParEqAdded();
}

void FenetreFonctions::removeParEq(ParEqWidget *widget)
{
    widget->close();
    parEqWidgets.removeOne(widget);
    delete widget;

    informations->emitAnimationUpdate();
}

void FenetreFonctions::addDataWidget()
{
    DataWidget *widget = new DataWidget(dataWidgets.size()+1, informations);
    connect(widget, SIGNAL(removeMe(DataWidget*)), this, SLOT(removeDataWidget(DataWidget*)));

    dataWidgets << widget;
    ui->dataWidgetsContainerLayout->addWidget(widget);
}

void FenetreFonctions::removeDataWidget(DataWidget *widget)
{
    dataWidgets.removeOne(widget);
    widget->close();

    delete widget;
}

FenetreFonctions::~FenetreFonctions()
{
    delete ui;   
}
