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

    xLogBase = new NumberLineEdit(false, info->getFuncsList());
    xLogBase->setMaximumHeight(27);

    yLogBase = new NumberLineEdit(false, info->getFuncsList());
    yLogBase->setMaximumHeight(27);

    ui->gridLayout->addWidget(Xmin,0,1);
    ui->gridLayout->addWidget(Xmax,1,1);
    ui->gridLayout->addWidget(Xstep,2,1);

    ui->gridLayout->addWidget(Ymin,0,3);
    ui->gridLayout->addWidget(Ymax,1,3);
    ui->gridLayout->addWidget(Ystep,2,3);

    ui->xAxisLayout->addWidget(xLogBase);
    ui->xAxisLayout->addStretch();

    ui->yAxisLayout->addWidget(yLogBase);
    ui->yAxisLayout->addStretch();

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->orthonormal);
    buttonGroup->addButton(ui->logScale);

    connect(Xmax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Xmin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Xstep, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(Ymax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Ymin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(Ystep, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->grid, SIGNAL(toggled(bool)), this, SLOT(manageWidgetStates()));
    connect(ui->specialView, SIGNAL(toggled(bool)), this, SLOT(manageWidgetStates()));
    connect(ui->logScale, SIGNAL(toggled(bool)), this, SLOT(manageWidgetStates()));
    connect(ui->subgrid, SIGNAL(toggled(bool)), this, SLOT(manageWidgetStates()));

    connect(ui->standardView, SIGNAL(released()), this, SLOT(standardView()));
    connect(ui->orthonormal, SIGNAL(clicked(bool)), information, SLOT(setOrthonormal(bool)));

    connect(ui->buttonApply, SIGNAL(released()), this, SLOT(apply()));

    connect(info, SIGNAL(updateOccured()), this, SLOT(updateWidgets()));

    updateWidgets();
}

void RangeAdjustments::setOrthonormal(bool state)
{   
    Ymax->setEnabled(!state);
    Ymin->setEnabled(!state);
    Ystep->setEnabled(!state);
}

void RangeAdjustments::setXLogScale(bool state)
{

}

void RangeAdjustments::setYLogScale(bool state)
{

}

void RangeAdjustments::manageWidgetStates()
{
    ui->subgrid->setEnabled(ui->grid->isChecked());
    ui->divsWidget->setEnabled(ui->subgrid->isChecked() && ui->subgrid->isEnabled());
    ui->logBaseWidget->setEnabled(ui->logScale->isChecked());
    ui->orthonormal->setEnabled(ui->specialView->isChecked());
    ui->logScale->setEnabled(ui->specialView->isChecked());
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

    xLogBase->setNumber(10);
    yLogBase->setNumber(10);
}

void RangeAdjustments::apply()
{
    GraphSettings graphSettings = information->getGraphSettings();

    if(!Xmax->isValid() || !Xmin->isValid() || !Xstep->isValid() ||
            !Ymax->isValid() || !Ymin->isValid() || !Ystep->isValid())
        return;

    if(ui->orthonormal->isChecked())
        Ystep->setNumber(Xstep->getValue());


    graphSettings.view.Xmax = Xmax->getValue();
    graphSettings.view.Xmin = Xmin->getValue();
    graphSettings.gridSettings.xGridStep = Xstep->getValue();

    graphSettings.view.Ymax = Ymax->getValue();
    graphSettings.view.Ymin = Ymin->getValue();
    graphSettings.gridSettings.yGridStep = Ystep->getValue();


    if(graphSettings.view.Xmin >= graphSettings.view.Xmax)
    {
        messageBox->setText(tr("X<sub>min</sub> must be smaller than X<sub>max</sub>"));
        messageBox->exec();
        return;
    }
    if(graphSettings.view.Ymin >= graphSettings.view.Ymax)
    {
        messageBox->setText(tr("Y<sub>min</sub> must be smaller than Y<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(graphSettings.view.Ymax - graphSettings.view.Ymin < MIN_RANGE || graphSettings.view.Xmax - graphSettings.view.Xmin < MIN_RANGE)
    {
        messageBox->setText(tr("The view graphSettings.view is too tight for ZeGrapher to distinguish between the upper and lower values."));
        messageBox->exec();
        return;
    }

    graphSettings.view.viewType = ZeScaleType::LINEAR;

    if(ui->specialView->isChecked())
    {
        if(ui->orthonormal->isChecked())
            graphSettings.view.viewType = ZeScaleType::LINEAR_ORTHONORMAL;
        else if(ui->logScale->isChecked())
        {
            if(ui->xLogScale->isChecked() && ui->yLogScale->isChecked())
                graphSettings.view.viewType = ZeScaleType::XY_LOG;
            else if(ui->yLogScale->isChecked())
                graphSettings.view.viewType = ZeScaleType::Y_LOG;
            else graphSettings.view.viewType = ZeScaleType::X_LOG;

        }
    }

    information->setGraphSettings(graphSettings);
}

void RangeAdjustments::updateWidgets()
 {
    manageWidgetStates();

    ZeGraphView window = information->getGraphView();
    ZeGridSettings gridSettings = information->getGridSettings();

    Xmax->setNumber(window.Xmax);
    Xmin->setNumber(window.Xmin);
    Xstep->setNumber(gridSettings.xGridStep);

    Ymax->setNumber(window.Ymax);
    Ymin->setNumber(window.Ymin);
    Ystep->setNumber(gridSettings.yGridStep);

    ui->orthonormal->setChecked(information->isOrthonormal());
    setOrthonormal(information->isOrthonormal());


 }


RangeAdjustments::~RangeAdjustments()
{
    delete ui;
    delete calculator;
}
