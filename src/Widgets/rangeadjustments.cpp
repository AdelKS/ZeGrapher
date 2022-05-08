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

/* TODO: Take care of the orthonormal setting */

RangeAdjustments::RangeAdjustments(QList<FuncCalculator*> funcsList, Information *info, QWidget *parent): QWidget(parent)
{
    information = info;

    Xmin = new NumberLineEdit(false, funcsList);
    Xmin->setMaximumHeight(27);
    Xmin->setValue(graphRange.x.min);    
    Xmin->setAlignment(Qt::AlignHCenter);

    Xmax = new NumberLineEdit(false, funcsList);
    Xmax->setMaximumHeight(27);
    Xmax->setValue(graphRange.x.max);
    Xmax->setAlignment(Qt::AlignHCenter);

    Ymin = new NumberLineEdit(false, funcsList);
    Ymin->setMaximumHeight(27);
    Ymin->setValue(graphRange.y.min);    
    Ymin->setAlignment(Qt::AlignHCenter);

    Ymax = new NumberLineEdit(false, funcsList);
    Ymax->setMaximumHeight(27);
    Ymax->setValue(graphRange.y.max);
    Ymax->setAlignment(Qt::AlignHCenter);

    QVBoxLayout *xminLayout = new QVBoxLayout();
    xminLayout->addWidget(new QLabel("x<sub>min</sub>"), 0, Qt::AlignHCenter);
    xminLayout->addWidget(Xmin, 0, Qt::AlignHCenter);

    QVBoxLayout *xmaxLayout = new QVBoxLayout();
    xmaxLayout->addWidget(new QLabel("x<sub>max</sub>"), 0, Qt::AlignHCenter);
    xmaxLayout->addWidget(Xmax, 0, Qt::AlignHCenter);

    QVBoxLayout *ymaxLayout = new QVBoxLayout();
    ymaxLayout->addWidget(new QLabel("y<sub>max</sub>"), 0, Qt::AlignHCenter);
    ymaxLayout->addWidget(Ymax, 0, Qt::AlignHCenter);

    QVBoxLayout *yminLayout = new QVBoxLayout();
    yminLayout->addWidget(new QLabel("y<sub>min</sub>"), 0, Qt::AlignHCenter);
    yminLayout->addWidget(Ymin, 0, Qt::AlignHCenter);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addLayout(xminLayout, 1, 0);
    gridLayout->addLayout(xmaxLayout, 1, 2);
    gridLayout->addLayout(yminLayout, 2, 1);
    gridLayout->addLayout(ymaxLayout, 0, 1);

    QHBoxLayout *buttonsBox = new QHBoxLayout();
    buttonsBox->setMargin(0);

    orthonormalButton = new QPushButton();
    orthonormalButton->setFixedSize(30, 30);
    orthonormalButton->setIconSize(QSize(26, 26));
    orthonormalButton->setIcon(QIcon(":/icons/orthonormalView.svg"));

    resetButton = new QPushButton();
    resetButton->setFixedSize(30, 30);
    resetButton->setIconSize(QSize(26, 26));
    resetButton->setIcon(QIcon(":/icons/resetToDefaultView.svg"));

    buttonsBox->addStretch();
    buttonsBox->addWidget(orthonormalButton);
    buttonsBox->addWidget(resetButton);

    gridLayout->addLayout(buttonsBox, 0, 2);

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    loadDefaults();
    information->setGraphRange(graphRange);

    connect(Xmax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Xmin, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(Ymax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Ymin, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(resetButton, SIGNAL(released()), this, SIGNAL(resetToDefaultView()));
}

void RangeAdjustments::loadDefaults()
{
    GraphRange defaultRange;
    defaultRange.x.max = defaultRange.y.max = 10;
    defaultRange.x.min = defaultRange.y.min = -10;

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

}
