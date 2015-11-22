/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#include "Windows/windowfunctions.h"
#include "ui_windowfunctions.h"


WindowFunctions::WindowFunctions(Information *info)
{   
    ui = new Ui::WindowFunctions;
    ui->setupUi(this);   

    parEqController = new ParEqController(&parEqWidgets);

    connect(ui->periodSpinBox, SIGNAL(valueChanged(int)), parEqController, SLOT(setIncrPeriod(int)));
    connect(ui->freqSpinBox, SIGNAL(valueChanged(int)), parEqController, SLOT(setUpdateFreq(int)));

    connect(parEqController, SIGNAL(animationUpdate()), info, SLOT(emitAnimationUpdate()));

    setWindowTitle(tr("Plot"));
    setWindowIcon(QIcon(":/icons/functions.png"));

    funcNames << 'f' << 'g' << 'h'<< 'p' << 'r'<< 'm';
    seqNames << 'u' << 'v' << 'l' << 'w' << 'q' << 't';

    setInfoClass(info);//must be before addFunctions and addSequences, because they use information class
    addFunctions();
    addSequences();

    ui->periodSpinBox->setValue(INIT_INCR_PERIOD);
    ui->freqSpinBox->setValue(INIT_FREQ);



    connect(ui->buttonPlot, SIGNAL(released()), this, SLOT(draw()));
    connect(ui->addLine, SIGNAL(released()), this, SLOT(addStraightline()));
    connect(ui->addTangent, SIGNAL(released()), this, SLOT(addTangent()));
    connect(ui->addParEq, SIGNAL(released()), this, SLOT(addParEq()));
    connect(ui->addDataWidget, SIGNAL(released()), this, SLOT(addDataWidget()));

    connect(ui->keyboardButton, SIGNAL(released()), this, SLOT(keyboardButtonClicked()));
}

void WindowFunctions::addFunctions()
{   
    for(int i = 0 ; i < funcNames.size() ; i++)
    {
        FuncWidget *widget = new FuncWidget(funcNames[i], i, Qt::black, window());
        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), information, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState(int)), this, SLOT(newFuncParametricState()));

        ui->funcWidgetsLayout->addWidget(widget);

        funcCalcs << widget->getCalculator();
        funcWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->funcWidgetsLayout->addWidget(frame);
    }

    information->setFunctionsList(funcCalcs);

    for(int i = 0 ; i < funcCalcs.size() ; i++)
    {
        funcWidgets[i]->setFuncWidgets(funcWidgets);
        funcWidgets[i]->setFuncsCalcsList(funcCalcs);
        funcCalcs[i]->setFuncsPointers(funcCalcs);
    }
}

void WindowFunctions::addSequences()
{
    for(int i = 0 ; i < seqNames.size() ; i++)
    {
        SeqWidget *widget = new SeqWidget(seqNames[i], i, Qt::black);
        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), information, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState()), this, SLOT(newSeqParametricState()));

        ui->seqWidgetsLayout->addWidget(widget);

        seqCalcs << widget->getCalculator();
        seqWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->seqWidgetsLayout->addWidget(frame);
    }

    information->setSequencesList(seqCalcs);

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

void WindowFunctions::newFuncParametricState()
{
    for(int i = 0 ; i < funcWidgets.size() ; i++)
        funcWidgets[i]->checkCalledFuncsParametric();

    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void WindowFunctions::newSeqParametricState()
{
    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void WindowFunctions::setInfoClass(Information *info)
{
    information = info;   
    information->setParEqsListPointer(&parEqWidgets);
    information->setTangentsListPointer(&tangentWidgets);
    information->setStraightLinesListPointer(&straightlineWidgets);   
}

void WindowFunctions::draw()
{    
    validateFunctions();
    validateSequences();
    validateLines();
    validateParametricEquations();
    information->emitUpdateSignal();
}

void WindowFunctions::validateFunctions()
{
    for(int i = 0 ; i < funcWidgets.size(); i++)
        funcWidgets[i]->firstValidation();

    for(int i = 0 ; i < funcWidgets.size(); i++)
        funcWidgets[i]->secondValidation();
}

void WindowFunctions::validateSequences()
{
    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->firstValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->secondValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->thirdValidation();
}

void WindowFunctions::validateLines()
{
    for(int i = 0 ; i < straightlineWidgets.size(); i++)
        straightlineWidgets[i]->validate();

    for(int i = 0 ; i < tangentWidgets.size(); i++)
        tangentWidgets[i]->validate();

}

void WindowFunctions::validateParametricEquations()
{
    for(int i = 0 ; i < parEqWidgets.size(); i++)
        parEqWidgets[i]->apply();
}

void WindowFunctions::keyboardButtonClicked()
{
    emit displayKeyboard();
}

void WindowFunctions::addTangent()
{
    TangentWidget *tangent = new TangentWidget(tangentWidgets.size(), funcCalcs, funcWidgets);
    tangentWidgets << tangent;

    connect(tangent, SIGNAL(removeMe(TangentWidget*)), this, SLOT(removeTangent(TangentWidget*)));
    connect(tangent, SIGNAL(returnPressed()), this, SLOT(draw()));
    connect(tangent, SIGNAL(drawStateChanged()), information, SLOT(emitDrawStateUpdate()));

    ui->linesLayout->addWidget(tangent);
}

void WindowFunctions::removeTangent(TangentWidget *widget)
{    
    for(int i = tangentWidgets.indexOf(widget) + 1 ; i < tangentWidgets.size() ; i++)
        tangentWidgets[i]->changeID(i-1);

    tangentWidgets.removeOne(widget);
    widget->close();
    delete widget;

    information->emitUpdateSignal();
}

void WindowFunctions::addStraightline()
{
    StraightLineWidget *line = new StraightLineWidget(straightlineWidgets.size(), funcCalcs);
    straightlineWidgets << line;

    connect(line, SIGNAL(removeMe(StraightLineWidget*)), this, SLOT(removeStraightline(StraightLineWidget*)));
    connect(line, SIGNAL(returnPressed()), this, SLOT(draw()));
    connect(line, SIGNAL(drawStateChanged()), information, SLOT(emitDrawStateUpdate()));

    ui->linesLayout->addWidget(line);
}

void WindowFunctions::removeStraightline(StraightLineWidget *widget)
{
    for(int i = straightlineWidgets.indexOf(widget) + 1; i < straightlineWidgets.size(); i++)
        straightlineWidgets[i]->changeID(i-1);

    straightlineWidgets.removeOne(widget);
    widget->close();
    delete widget;

}

void WindowFunctions::addParEq()
{
    ParEqWidget *widget = new ParEqWidget(parEqWidgets.size(), funcCalcs);
    connect(widget, SIGNAL(removeClicked(ParEqWidget*)), this, SLOT(removeParEq(ParEqWidget*)));
    connect(widget, SIGNAL(updateRequest()), information, SLOT(emitDrawStateUpdate()));
    connect(widget, SIGNAL(animationUpdateRequest()), information, SLOT(emitAnimationUpdate()));
    connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));

    parEqWidgets << widget;
    ui->parEqLayout->addWidget(widget);

    parEqController->newParEqAdded();
}

void WindowFunctions::removeParEq(ParEqWidget *widget)
{
    widget->close();
    parEqWidgets.removeOne(widget);
    delete widget;

    information->emitAnimationUpdate();
}

void WindowFunctions::addDataWidget()
{
    information->addDataList();

    DataWidget *widget = new DataWidget(dataWidgets.size(), information);
    connect(widget, SIGNAL(removeMe(DataWidget*)), this, SLOT(removeDataWidget(DataWidget*)));

    dataWidgets << widget;
    ui->dataWidgetsContainerLayout->addWidget(widget);
}

void WindowFunctions::removeDataWidget(DataWidget *widget)
{
    information->removeDataList(dataWidgets.indexOf(widget));

    for(int i = dataWidgets.indexOf(widget)+1; i < dataWidgets.size(); i++)
        dataWidgets[i]->setWidgetNum(i-1);

    dataWidgets.removeOne(widget);
    widget->close();

    delete widget;
}

 void WindowFunctions::closeAllOpenedWindows()
 {
     for(int i = 0 ; i < dataWidgets.size(); i++)
     {
         dataWidgets[i]->closeDataWindow();
     }
 }

WindowFunctions::~WindowFunctions()
{
}
