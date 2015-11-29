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



#include "Windows/rangeadjustments.h"
#include "ui_rangeadjustments.h"

RangeAdjustments::RangeAdjustments(Information *info)
{
    information = info;


    calculator = new ExprCalculator(false, information->getFuncsList());

    ui = new Ui::RangeAdjustments;
    ui->setupUi(this);

    Xmin = new NumberLineEdit(false, info->getFuncsList());
    Xmin->setMaximumHeight(27);

    Xmax = new NumberLineEdit(false, info->getFuncsList());
    Xmax->setMaximumHeight(27);

    Xstep = new NumberLineEdit(false, info->getFuncsList());
    Xstep->setMaximumHeight(27);

    Ymin = new NumberLineEdit(false, info->getFuncsList());
    Ymin->setMaximumHeight(27);

    Ymax = new NumberLineEdit(false, info->getFuncsList());
    Ymax->setMaximumHeight(27);

    Ystep = new NumberLineEdit(false, info->getFuncsList());
    Ystep->setMaximumHeight(27);

    ui->gridLayout->addWidget(Xmin,0,1);
    ui->gridLayout->addWidget(Xmax,1,1);
    ui->gridLayout->addWidget(Xstep,2,1);

    ui->gridLayout->addWidget(Ymin,0,3);
    ui->gridLayout->addWidget(Ymax,1,3);
    ui->gridLayout->addWidget(Ystep,2,3);

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    connect(Xmax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Xmin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Xstep, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(Ymax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Ymin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Ystep, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->standardView, SIGNAL(released()), this, SLOT(standardView()));
    connect(ui->orthonormal, SIGNAL(clicked(bool)), information, SLOT(setOrthonormal(bool)));

    connect(ui->buttonApply, SIGNAL(released()), this, SLOT(apply()));

    connect(info, SIGNAL(updateOccured()), this, SLOT(updateWidgets()));

    updateWidgets();
}

void RangeAdjustments::orthonormal(bool state)
{   
    Ymax->setEnabled(!state);
    Ymin->setEnabled(!state);
    Ystep->setEnabled(!state);
}

void RangeAdjustments::resetToStandardView()
{
    standardView();
    apply();
}

void RangeAdjustments::standardView()
{
    Xmax->setNumber(10);
    Xmin->setNumber(-10);
    Xstep->setNumber(1);

    Ymax->setNumber(10);
    Ymin->setNumber(-10);
    Ystep->setNumber(1);
}

void RangeAdjustments::apply()
{
    if(!Xmax->isValid() || !Xmin->isValid() || !Xstep->isValid() ||
            !Ymax->isValid() || !Ymin->isValid() || !Ystep->isValid())
        return;

    if(ui->orthonormal->isChecked())
        Ystep->setNumber(Xstep->getValue());

    GraphRange range;

    range.Xmax = Xmax->getValue();
    range.Xmin = Xmin->getValue();
    range.Xscale = Xstep->getValue();

    range.Ymax = Ymax->getValue();
    range.Ymin = Ymin->getValue();
    range.Yscale = Ystep->getValue();


    if(range.Xmin >= range.Xmax)
    {
        messageBox->setText(tr("X<sub>min</sub> must be smaller than X<sub>max</sub>"));
        messageBox->exec();
        return;
    }
    if(range.Ymin >= range.Ymax)
    {
        messageBox->setText(tr("Y<sub>min</sub> must be smaller than Y<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(range.Ymax - range.Ymin < MIN_RANGE || range.Xmax - range.Xmin < MIN_RANGE)
    {
        messageBox->setText(tr("The view range is too tight for ZeGrapher to distinguish between the upper and lower values."));
        messageBox->exec();
        return;
    }

    information->setOrthonormal(ui->orthonormal->isChecked());
    information->setRange(range);
}

void RangeAdjustments::updateWidgets()
 {
     GraphRange window = information->getRange();

     Xmax->setNumber(window.Xmax);
     Xmin->setNumber(window.Xmin);
     Xstep->setNumber(window.Xscale);

     Ymax->setNumber(window.Ymax);
     Ymin->setNumber(window.Ymin);
     Ystep->setNumber(window.Yscale);

     ui->orthonormal->setChecked(information->isOrthonormal());
     orthonormal(information->isOrthonormal());
 }


RangeAdjustments::~RangeAdjustments()
{
    delete ui;
    delete calculator;
}
