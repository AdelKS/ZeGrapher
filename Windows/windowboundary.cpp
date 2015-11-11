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

WindowBoundary::WindowBoundary(Informations *info)
{
    information = info;


    calculator = new ExprCalculator(false, information->getFuncsList());

    ui = new Ui::WindowBoundary;
    ui->setupUi(this);

    messageBox = new QMessageBox(this);
    messageBox->setWindowTitle(tr("Erreur"));
    messageBox->setIcon(QMessageBox::Warning);

    connect(ui->Xmax, SIGNAL(returnPressed()), this, SLOT(appliquer()));
    connect(ui->Xmin, SIGNAL(returnPressed()), this, SLOT(appliquer()));
    connect(ui->Xpas, SIGNAL(returnPressed()), this, SLOT(appliquer()));

    connect(ui->Ymax, SIGNAL(returnPressed()), this, SLOT(appliquer()));
    connect(ui->Ymin, SIGNAL(returnPressed()), this, SLOT(appliquer()));
    connect(ui->Ypas, SIGNAL(returnPressed()), this, SLOT(appliquer()));

    connect(ui->standardView, SIGNAL(released()), this, SLOT(standardView()));
    connect(ui->orthonormal, SIGNAL(clicked(bool)), information, SLOT(setOrthonormal(bool)));

    connect(ui->boutonAppliquer, SIGNAL(released()), this, SLOT(appliquer()));

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
    appliquer();
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

void WindowBoundary::appliquer()
{
    GraphRange range;
    bool ok = false;

    range.Xmax = calculator->calculateExpression(ui->Xmax->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite en ") + "X<sub>max</sub>");
        messageBox->exec();
        return;
    }
    range.Xmin = calculator->calculateExpression(ui->Xmin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite en ") + "X<sub>min</sub>");
        messageBox->exec();
        return;
    }
    range.Ymax = calculator->calculateExpression(ui->Ymax->text(),ok);
    if(ok == false)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite en ") + "Y<sub>max</sub>");
        messageBox->exec();
        return;
    }
    range.Ymin = calculator->calculateExpression(ui->Ymin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite en ") + "X<sub>min</sub>");
        messageBox->exec();
        return;
    }
    range.Xscale = calculator->calculateExpression(ui->Xpas->text(), ok);
    if(ok == false || range.Xscale <= 0)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite pour le pas des X."));
        messageBox->exec();
        return;
    }    

    if(ui->orthonormal->isChecked())
        ui->Ypas->setText(ui->Xpas->text());

    range.Yscale = calculator->calculateExpression(ui->Ypas->text(), ok);
    if(ok == false || range.Yscale <= 0)
    {
        messageBox->setText(tr("Erreur lors de l'evaluation de l'expression écrite pour le pas des Y."));
        messageBox->exec();
        return;
    }

    if(range.Xmin >= range.Xmax)
    {
        messageBox->setText(tr("X<sub>min</sub> doit être plus petit que X<sub>max</sub>"));
        messageBox->exec();
        return;
    }
    if(range.Ymin >= range.Ymax)
    {
        messageBox->setText(tr("Y<sub>min</sub> doit être plus petit que Y<sub>max</sub>"));
        messageBox->exec();
        return;
    }

    if(range.Ymax - range.Ymin < MIN_RANGE || range.Xmax - range.Xmin < MIN_RANGE)
    {
        messageBox->setText(tr("Les bornes sont trop proches pour être traitées comme différentes par ZeGrapher."));
        messageBox->exec();
        return;
    }

    information->setOrthonormal(ui->orthonormal->isChecked());
    information->setRange(range);
}

void WindowBoundary::updateWidgets()
 {
     GraphRange fenetre = information->getRange();

     ui->Xmax->setText(QString::number(fenetre.Xmax));
     ui->Xmin->setText(QString::number(fenetre.Xmin));
     ui->Xpas->setText(QString::number(fenetre.Xscale));

     ui->Ymax->setText(QString::number(fenetre.Ymax));
     ui->Ymin->setText(QString::number(fenetre.Ymin));
     ui->Ypas->setText(QString::number(fenetre.Yscale));

     ui->orthonormal->setChecked(information->isOrthonormal());
     orthonormal(information->isOrthonormal());
 }


WindowBoundary::~WindowBoundary()
{
    delete ui;
    delete calculator;
}
