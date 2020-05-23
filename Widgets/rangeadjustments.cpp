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

#include "Widgets/rangeadjustments.h"
#include "ui_rangeadjustments.h"

#include <boost/fusion/container/vector/detail/begin_impl.hpp>

/* TODO: Take care of the orthonormal setting */

RangeAdjustments::RangeAdjustments(QList<FuncCalculator*> funcsList, Information *info, QWidget *parent): QWidget(parent)
{
    information = info;

    ui = new Ui::RangeAdjustments;
    ui->setupUi(this);

    Xmin = new NumberLineEdit(false, funcsList);
    Xmin->setMaximumHeight(27);
    Xmin->setValue(graphRange.x.min);

    Xmax = new NumberLineEdit(false, funcsList);
    Xmax->setMaximumHeight(27);
    Xmax->setValue(graphRange.x.max);  

    Ymin = new NumberLineEdit(false, funcsList);
    Ymin->setMaximumHeight(27);
    Ymin->setValue(graphRange.y.min);

    Ymax = new NumberLineEdit(false, funcsList);
    Ymax->setMaximumHeight(27);
    Ymax->setValue(graphRange.y.max);   

    ui->xForm->addRow(new QLabel("x<sub>min</sub>"), Xmin);
    ui->xForm->addRow(new QLabel("x<sub>max</sub>"), Xmax);  

    ui->yForm->addRow(new QLabel("y<sub>min</sub>"), Ymin);
    ui->yForm->addRow(new QLabel("y<sub>max</sub>"), Ymax);

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    loadDefaults();

    connect(Xmax, SIGNAL(returnPressed()), this, SLOT(processUserInput()));
    connect(Xmin, SIGNAL(returnPressed()), this, SLOT(processUserInput()));

    connect(Ymax, SIGNAL(returnPressed()), this, SLOT(processUserInput()));
    connect(Ymin, SIGNAL(returnPressed()), this, SLOT(processUserInput()));

    connect(ui->resetView, SIGNAL(released()), this, SIGNAL(resetToDefaultView()));
    connect(ui->apply, SIGNAL(released()), this, SLOT(processUserInput()));
    connect(ui->apply, SIGNAL(released()), this, SLOT(onTickIntervalChange()));
}

void RangeAdjustments::hideViewOptions(bool hide)
{
    ui->viewOptionsWidget->setHidden(hide);
}

void RangeAdjustments::loadDefaults()
{
    GraphRange defaultRange;
    defaultRange.x.max = defaultRange.y.max = 10;
    defaultRange.x.max = defaultRange.y.max = -10;

    setGraphRange(defaultRange);
}


void RangeAdjustments::resetToStandardView()
{
    // TODO
}

GraphRange RangeAdjustments::getRange()
{
    return graphRange;
}

void RangeAdjustments::disableUserInput(bool disable)
{
    Xmin->setDisabled(disable);
    Xmax->setDisabled(disable);
    Ymin->setDisabled(disable);
    Ymax->setDisabled(disable);
    ui->viewOptionsWidget->setDisabled(disable);
}

void RangeAdjustments::setGraphRange(const GraphRange &range)
{
    const QSignalBlocker blocker1(Xmin);
    const QSignalBlocker blocker2(Xmax);

    const QSignalBlocker blocker3(Ymin);
    const QSignalBlocker blocker4(Ymax);

    Xmin->setValue(range.x.min);
    Xmax->setValue(range.x.max);

    Ymin->setValue(range.y.min);
    Ymax->setValue(range.y.max);

    graphRange = range;
}

void RangeAdjustments::apply()
{
    processUserInput();

    information->setGraphRange(graphRange);
}

void RangeAdjustments::processUserInput()
{
    ZeViewSettings viewSettings = information->getViewSettings();

    Xmin->checkVal();
    Ymin->checkVal();

    Xmax->checkVal();
    Ymax->checkVal();

    if(!Xmax->isValid() || !Xmin->isValid() || !Ymax->isValid() || !Ymin->isValid())
        return;

    GraphRange newGraphRange;

    newGraphRange.x.max = Xmax->getValue();
    newGraphRange.x.min = Xmin->getValue();

    newGraphRange.y.max = Ymax->getValue();
    newGraphRange.y.min = Ymin->getValue();

    if(newGraphRange.x.min >= newGraphRange.x.max)
    {
        messageBox->setText(tr("x<sub>min</sub> must be smaller than x<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(newGraphRange.x.min <= 0 && viewSettings.axes.x.axisType == ZeAxisType::LOG)
    {
        messageBox->setText(tr("x<sub>min</sub> must strictly positive when on a log representation"));
        messageBox->exec();
        return;
    }

    if(newGraphRange.y.min >= newGraphRange.y.max)
    {
        messageBox->setText(tr("Y<sub>min</sub> must be smaller than Y<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(newGraphRange.y.min <= 0 && viewSettings.axes.y.axisType == ZeAxisType::LOG)
    {
        messageBox->setText(tr("y<sub>min</sub> must strictly positive when on a log representation"));
        messageBox->exec();
        return;
    }

    if(newGraphRange.y.amplitude() < MIN_AMPLITUDE || newGraphRange.x.amplitude() < MIN_AMPLITUDE)
    {
        messageBox->setText(tr("The requested view window is too small."));
        messageBox->exec();
        return;
    }

    graphRange = newGraphRange;
}

void RangeAdjustments::setOrthonormal(bool state)
{
    Ymax->setEnabled(!state);
    Ymin->setEnabled(!state);
}

RangeAdjustments::~RangeAdjustments()
{
    delete ui;
}
