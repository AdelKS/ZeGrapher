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

#include "Widgets/mathobjectsinput.h"
#include "information.h"
#include "ui_mathobjectsinput.h"


MathObjectsInput::MathObjectsInput(QWidget *parent):
    QWidget(parent),
    ui(new Ui::MathObjectsInput),
    parEqWidgets(),
    parEqController(&parEqWidgets)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);

    helpWindow.resize(800,600);
    helpWindow.setParent(this);
    helpWindow.setWindowFlags(Qt::Window);

    connect(ui->periodSpinBox, SIGNAL(valueChanged(int)), &parEqController, SLOT(setIncrPeriod(int)));
    connect(ui->freqSpinBox, SIGNAL(valueChanged(int)), &parEqController, SLOT(setUpdateFreq(int)));

    connect(&parEqController, SIGNAL(animationUpdate()), &information, SLOT(emitAnimationUpdate()));

    setWindowTitle(tr("Plot"));
    setWindowIcon(QIcon(":/icons/functions.png"));

    funcNames << 'f' << 'g' << 'h'<< 'p' << 'r'<< 'm';
    seqNames << 'u' << 'v' << 'l' << 'w' << 'q' << 't';

    setInfoClass();//must be before addFunctions and addSequences, because they use information class
    addFunctions();
    addSequences();

    ui->periodSpinBox->setValue(INIT_INCR_PERIOD);
    ui->freqSpinBox->setValue(INIT_FREQ);

    connect(ui->buttonPlot, SIGNAL(released()), this, SLOT(draw()));
    connect(ui->addParEq, SIGNAL(released()), this, SLOT(addParEq()));
    connect(ui->addDataWidget, SIGNAL(released()), this, SLOT(addDataWidget()));

    connect(ui->keyboardButton, SIGNAL(released()), this, SLOT(keyboardButtonClicked()));
}


QList<QColor> toColorsList(const QList<QVariant> &list)
{
    QList<QColor> colorsList;
    for(int i = 0 ; i < list.size() ; i++)
        colorsList << list.at(i).value<QColor>();
    return colorsList;
}

void MathObjectsInput::showDataHelpWindow()
{
    QSettings settings;
    if(settings.contains("app/language"))
    {
        QString language = settings.value("app/language").toString();
        if(language == "fr")
            helpWindow.setSource(QUrl("qrc:///Help/data_help_fr.html"));
        else if(language == "de")
            helpWindow.setSource(QUrl("qrc:///Help/data_help_de.html"));
        else if(language == "zh")
            helpWindow.setSource(QUrl("qrc:///Help/data_help_zh.html"));
        else helpWindow.setSource(QUrl("qrc:///Help/data_help_en.html"));
    }
    else
    {
        QLocale locale;
        if(locale.language() == QLocale::French)
            helpWindow.setSource(QUrl("qrc:///Help/data_help_fr.html"));
        else if(locale.language() == QLocale::German)
            helpWindow.setSource(QUrl("qrc:///Help/data_help_de.html"));
        else if(locale.language() == QLocale::Chinese)
            helpWindow.setSource(QUrl("qrc:///Help/data_help_zh.html"));
        else helpWindow.setSource(QUrl("qrc:///Help/data_help_en.html"));
    }

    helpWindow.setWindowTitle(tr("Help: data fill window."));

    helpWindow.show();
}

void MathObjectsInput::addFunctions()
{
    QSettings settings;
    QList<QColor> funcColors;

    if(settings.contains("functions/colors"))
        funcColors = toColorsList(settings.value("functions/colors").toList());

    for(int i = 0 ; i < funcNames.size() ; i++)
    {
        FuncWidget *widget;
        if(i < funcColors.size())
            widget = new FuncWidget(funcNames[i], i, funcColors.at(i));
        else widget = new FuncWidget(funcNames[i], i, information.getGraphSettings().defaultColor);

        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), &information, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState(int)), this, SLOT(newFuncParametricState()));

        ui->funcWidgetsLayout->addWidget(widget);

        funcCalcs << widget->getCalculator();
        funcWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->funcWidgetsLayout->addWidget(frame);
    }

    information.setFunctionsList(funcCalcs);

    for(int i = 0 ; i < funcCalcs.size() ; i++)
    {
        funcWidgets[i]->setFuncWidgets(funcWidgets);
        funcWidgets[i]->setFuncsCalcsList(funcCalcs);
        funcCalcs[i]->setFuncsPointers(funcCalcs);
    }
}

void MathObjectsInput::addSequences()
{
    QSettings settings;
    QList<QColor> seqColors;

    if(settings.contains("sequences/colors"))
        seqColors = toColorsList(settings.value("sequences/colors").toList());

    for(int i = 0 ; i < seqNames.size() ; i++)
    {
        SeqWidget *widget;

        if(i < seqColors.size())
            widget = new SeqWidget(seqNames[i], i, seqColors.at(i));
        else widget = new SeqWidget(seqNames[i], i, information.getGraphSettings().defaultColor);

        connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));
        connect(widget, SIGNAL(drawStateChanged()), &information, SLOT(emitDrawStateUpdate()));
        connect(widget, SIGNAL(newParametricState()), this, SLOT(newSeqParametricState()));

        ui->seqWidgetsLayout->addWidget(widget);

        seqCalcs << widget->getCalculator();
        seqWidgets << widget;

        QFrame *frame = new QFrame();
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);

        ui->seqWidgetsLayout->addWidget(frame);
    }

    information.setSequencesList(seqCalcs);

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

void MathObjectsInput::newFuncParametricState()
{
    for(int i = 0 ; i < funcWidgets.size() ; i++)
        funcWidgets[i]->checkCalledFuncsParametric();

    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void MathObjectsInput::newSeqParametricState()
{
    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqWidgets[i]->checkCalledFuncsSeqsParametric();
}

void MathObjectsInput::setInfoClass()
{
    information.setParEqsListPointer(&parEqWidgets);
}

void MathObjectsInput::draw()
{
    validateFunctions();
    validateSequences();
    validateParametricEquations();
    information.emitUpdateSignal();
}

void MathObjectsInput::validateFunctions()
{
    for(int i = 0 ; i < funcWidgets.size(); i++)
        funcWidgets[i]->firstValidation();

    for(int j = 0 ; j < funcWidgets.size(); j++)
        for(int i = 0 ; i < funcWidgets.size(); i++)
            funcWidgets[i]->secondValidation();
}

void MathObjectsInput::validateSequences()
{
    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->firstValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->secondValidation();

    for(int i = 0 ; i < seqWidgets.size(); i++)
        seqWidgets[i]->thirdValidation();
}

void MathObjectsInput::validateParametricEquations()
{
    for(int i = 0 ; i < parEqWidgets.size(); i++)
        parEqWidgets[i]->apply();
}

void MathObjectsInput::keyboardButtonClicked()
{
    emit displayKeyboard();
}

void MathObjectsInput::addParEq()
{
    ParEqWidget *widget = new ParEqWidget(parEqWidgets.size(), funcCalcs, information.getGraphSettings().defaultColor);
    connect(widget, SIGNAL(removeClicked(ParEqWidget*)), this, SLOT(removeParEq(ParEqWidget*)));
    connect(widget, SIGNAL(updateRequest()), &information, SLOT(emitDrawStateUpdate()));
    connect(widget, SIGNAL(animationUpdateRequest()), &information, SLOT(emitAnimationUpdate()));
    connect(widget, SIGNAL(returnPressed()), this, SLOT(draw()));

    parEqWidgets << widget;
    ui->parEqLayout->addWidget(widget);

    parEqController.newParEqAdded();
}

void MathObjectsInput::removeParEq(ParEqWidget *widget)
{
    widget->close();
    parEqWidgets.removeOne(widget);
    delete widget;

    information.emitAnimationUpdate();
}

void MathObjectsInput::addDataWidget()
{
    DataWidget *widget = new DataWidget(this);
    connect(widget, SIGNAL(removeMe(DataWidget*)), this, SLOT(removeDataWidget(DataWidget*)));
    connect(widget->getDataWindow(), SIGNAL(showHelpWindow()), this, SLOT(showDataHelpWindow()));

    information.addDataList(widget->getUserData());

    dataWidgets << widget;
    ui->dataWidgetsContainerLayout->addWidget(widget);
}

void MathObjectsInput::removeDataWidget(DataWidget *widget)
{
    information.removeDataList(widget->getUserData());

    dataWidgets.removeOne(widget);
    widget->close();

    delete widget;
}


QList<QVariant> toVariantList(const QList<QColor> &list)
{
    QList<QVariant> variantList;
    for(int i = 0 ; i < list.size() ; i++)
        variantList << list.at(i);
    return variantList;
}

void MathObjectsInput::saveColors()
{
    QSettings settings;
    QList<QColor> funcColors, seqColors;
    for(int i = 0 ; i < seqWidgets.size() ; i++)
        seqColors << seqWidgets.at(i)->getCalculator()->getColorSaver()->getColor(0);
    for(int i = 0 ; i < funcWidgets.size() ; i++)
        funcColors << funcWidgets.at(i)->getCalculator()->colorSaver.getColor(0);

    settings.setValue("functions/colors", toVariantList(funcColors));
    settings.setValue("sequences/colors", toVariantList(seqColors));
}


void MathObjectsInput::closeAllOpenedWindows()
{
    saveColors();
    helpWindow.close();

    for(int i = 0 ; i < dataWidgets.size(); i++)
    {
        dataWidgets[i]->closeDataWindow();
    }
}

MathObjectsInput::~MathObjectsInput()
{
   delete ui;
}
