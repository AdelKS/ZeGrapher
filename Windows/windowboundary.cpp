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



#include "Windows/windowboundary.h"
#include "ui_windowboundary.h"

WindowBoundary::WindowBoundary(Information *info)
{
    information = info;


    calculator = new ExprCalculator(false, information->getFuncsList());

    ui = new Ui::WindowBoundary;
    ui->setupUi(this);

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Error"));
    messageBox->setIcon(QMessageBox::Warning);

    connect(ui->Xmax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(ui->Xmin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(ui->Xpas, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->Ymax, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(ui->Ymin, SIGNAL(returnPressed()), this, SLOT(apply()));
    connect(ui->Ypas, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->standardView, SIGNAL(released()), this, SLOT(standardView()));
    connect(ui->orthonormal, SIGNAL(clicked(bool)), information, SLOT(setOrthonormal(bool)));

    connect(ui->buttonApply, SIGNAL(released()), this, SLOT(apply()));

    connect(info, SIGNAL(updateOccured()), this, SLOT(updateWidgets()));
}

void WindowBoundary::orthonormal(bool state)
{   
    ui->Ymax->setEnabled(!state);
    ui->Ymin->setEnabled(!state);
    ui->Ypas->setEnabled(!state);
}

void WindowBoundary::resetToStandardView()
{
    standardView();
    apply();
}

void WindowBoundary::standardView()
{
    ui->Xmax->setText("10");
    ui->Xmin->setText("-10");
    ui->Xpas->setText("1");

    ui->Ymax->setText("10");
    ui->Ymin->setText("-10");
    ui->Ypas->setText("1");
}

void WindowBoundary::apply()
{
    GraphRange range;
    bool ok = false;

    range.Xmax = calculator->calculateExpression(ui->Xmax->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Could not evaluate the typed expression for ") + "X<sub>max</sub>");
        messageBox->exec();
        return;
    }
    range.Xmin = calculator->calculateExpression(ui->Xmin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Error lors de l'evaluation de l'expression écrite en ") + "X<sub>min</sub>");
        messageBox->exec();
        return;
    }
    range.Ymax = calculator->calculateExpression(ui->Ymax->text(),ok);
    if(ok == false)
    {
        messageBox->setText(tr("Could not evaluate the typed expression for ") + "Y<sub>max</sub>");
        messageBox->exec();
        return;
    }
    range.Ymin = calculator->calculateExpression(ui->Ymin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Could not evaluate the typed expression for X step.") + "X<sub>min</sub>");
        messageBox->exec();
        return;
    }
    range.Xscale = calculator->calculateExpression(ui->Xpas->text(), ok);
    if(ok == false || range.Xscale <= 0)
    {
        messageBox->setText(tr("Could not evaluate the typed expression for X step."));
        messageBox->exec();
        return;
    }    

    if(ui->orthonormal->isChecked())
        ui->Ypas->setText(ui->Xpas->text());

    range.Yscale = calculator->calculateExpression(ui->Ypas->text(), ok);
    if(ok == false || range.Yscale <= 0)
    {
        messageBox->setText(tr("Could not evaluate the typed expression for Y step."));
        messageBox->exec();
        return;
    }

    if(range.Xmin >= range.Xmax)
    {
        messageBox->setText(tr("X<sub>min</sub> must be lower than X<sub>max</sub>"));
        messageBox->exec();
        return;
    }
    if(range.Ymin >= range.Ymax)
    {
        messageBox->setText(tr("Y<sub>min</sub> must be lower than Y<sub>max</sub>"));
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

void WindowBoundary::updateWidgets()
 {
     GraphRange window = information->getRange();

     ui->Xmax->setText(QString::number(window.Xmax));
     ui->Xmin->setText(QString::number(window.Xmin));
     ui->Xpas->setText(QString::number(window.Xscale));

     ui->Ymax->setText(QString::number(window.Ymax));
     ui->Ymin->setText(QString::number(window.Ymin));
     ui->Ypas->setText(QString::number(window.Yscale));

     ui->orthonormal->setChecked(information->isOrthonormal());
     orthonormal(information->isOrthonormal());
 }


WindowBoundary::~WindowBoundary()
{
    delete ui;
    delete calculator;
}
