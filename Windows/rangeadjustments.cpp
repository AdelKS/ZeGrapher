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




#include "Windows/rangeadjustments.h"
#include "ui_rangeadjustments.h"

RangeAdjustments::RangeAdjustments(QList<FuncCalculator*> funcsList, GraphRange graphRange, GraphTickIntervals tickIntervals, QWidget *parent): QWidget(parent)
{
    this->graphRange = graphRange;
    this->tickIntervals = tickIntervals;

    setWindowFlags(Qt::Window);

    calculator = new ExprCalculator(false, funcsList);

    ui = new Ui::RangeAdjustments;
    ui->setupUi(this);

    Xmin = new NumberLineEdit(false, funcsList);
    Xmin->setMaximumHeight(27);
    Xmin->setValue(graphRange.Xmin);

    Xmax = new NumberLineEdit(false, funcsList);
    Xmax->setMaximumHeight(27);
    Xmax->setValue(graphRange.Xmax);

    Xstep = new NumberLineEdit(false, funcsList);
    Xstep->setMaximumHeight(27);
    Xstep->setValue(tickIntervals.x);

    Ymin = new NumberLineEdit(false, funcsList);
    Ymin->setMaximumHeight(27);
    Ymin->setValue(graphRange.Ymin);

    Ymax = new NumberLineEdit(false, funcsList);
    Ymax->setMaximumHeight(27);
    Ymax->setValue(graphRange.Ymax);

    Ystep = new NumberLineEdit(false, funcsList);
    Ystep->setMaximumHeight(27);
    Ystep->setValue(tickIntervals.y);

    ui->xForm->addRow(new QLabel("x<sub>min</sub>"), Xmin);
    ui->xForm->addRow(new QLabel("x<sub>max</sub>"), Xmax);
    ui->xForm->addRow(new QLabel(tr("Δx<sub>tick</sub>")), Xstep);

    ui->yForm->addRow(new QLabel("y<sub>min</sub>"), Ymin);
    ui->yForm->addRow(new QLabel("y<sub>max</sub>"), Ymax);
    ui->yForm->addRow(new QLabel(tr("Δy<sub>tick</sub>")), Ystep);


    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    connect(Xmax, SIGNAL(returnPressed()), this, SLOT(onRangeChange()));
    connect(Xmin, SIGNAL(returnPressed()), this, SLOT(onRangeChange()));
    connect(Xstep, SIGNAL(returnPressed()), this, SLOT(onTickIntervalChange()));

    connect(Ymax, SIGNAL(returnPressed()), this, SLOT(onRangeChange()));
    connect(Ymin, SIGNAL(returnPressed()), this, SLOT(onRangeChange()));
    connect(Ystep, SIGNAL(returnPressed()), this, SLOT(onTickIntervalChange()));

    connect(ui->standardView, SIGNAL(released()), this, SLOT(standardView()));
    connect(ui->apply, SIGNAL(released()), this, SLOT(onRangeChange()));
    connect(ui->apply, SIGNAL(released()), this, SLOT(onTickIntervalChange()));
}

void RangeAdjustments::setMargin(int margin)
{
    ui->mainLayout->setMargin(margin);
}

void RangeAdjustments::hideViewOptions(bool hide)
{
    ui->viewOptionsWidget->setHidden(hide);
}

void RangeAdjustments::disableRangeWidgets(bool disable)
{
    Xmin->setDisabled(disable);
    Xmax->setDisabled(disable);
    Ymin->setDisabled(disable);
    Ymax->setDisabled(disable);
}

void RangeAdjustments::setGraphRange(GraphRange range)
{
    const QSignalBlocker blocker1(Xmin);
    const QSignalBlocker blocker2(Xmax);

    const QSignalBlocker blocker3(Ymin);
    const QSignalBlocker blocker4(Ymax);

    Xmin->setValue(range.Xmin);
    Xmax->setValue(range.Xmax);

    Ymin->setValue(range.Ymin);
    Ymax->setValue(range.Ymax);
}

void RangeAdjustments::setGraphTickIntervals(GraphTickIntervals tickIntervals)
{
    const QSignalBlocker blocker1(Xstep);
    const QSignalBlocker blocker2(Ystep);

    Xstep->setValue(tickIntervals.x);
    Ystep->setValue(tickIntervals.y);
}

void RangeAdjustments::onRangeChange()
{
    Xmin->checkVal();
    Ymin->checkVal();

    Xmax->checkVal();
    Ymax->checkVal();

    if(!Xmax->isValid() || !Xmin->isValid() || !Ymax->isValid() || !Ymin->isValid())
        return;

    if(ui->orthonormal->isChecked())
        Ystep->setValue(Xstep->getValue());

    GraphRange oldGraphRange = graphRange;

    graphRange.Xmax = Xmax->getValue();
    graphRange.Xmin = Xmin->getValue();

    graphRange.Ymax = Ymax->getValue();
    graphRange.Ymin = Ymin->getValue();

    if(graphRange.Xmin >= graphRange.Xmax)
    {
        messageBox->setText(tr("X<sub>min</sub> must be smaller than X<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(graphRange.Ymin >= graphRange.Ymax)
    {
        messageBox->setText(tr("Y<sub>min</sub> must be smaller than Y<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(graphRange.Ymax - graphRange.Ymin < MIN_RANGE || graphRange.Xmax - graphRange.Xmin < MIN_RANGE)
    {
        messageBox->setText(tr("The view range is too tight for ZeGrapher to distinguish between the upper and lower values."));
        messageBox->exec();
        return;
    }

    if(oldGraphRange != graphRange)
    {
        emit graphRangeChanged(graphRange);
    }
}

void RangeAdjustments::onTickIntervalChange()
{
    Xstep->checkVal();
    Ystep->checkVal();

    if(!Xstep->isValid() || !Ystep->isValid())
        return;

    GraphTickIntervals oldTickInterval = tickIntervals;

    tickIntervals.x = Xstep->getValue();
    tickIntervals.y = Ystep->getValue();

    if(tickIntervals != oldTickInterval)
    {
        emit graphTickIntervalsChanged(tickIntervals);
    }
}

void RangeAdjustments::setOrthonormal(bool state)
{   
    Ymax->setEnabled(!state);
    Ymin->setEnabled(!state);
    Ystep->setEnabled(!state);
}

void RangeAdjustments::resetToStandardView()
{
    standardView();
    onRangeChange();
    onTickIntervalChange();
}

void RangeAdjustments::standardView()
{
    Xmax->setValue(10);
    Xmin->setValue(-10);
    Xstep->setValue(1);

    Ymax->setValue(10);
    Ymin->setValue(-10);
    Ystep->setValue(1);
}

RangeAdjustments::~RangeAdjustments()
{
    delete ui;
    delete calculator;
}
