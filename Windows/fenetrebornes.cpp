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



#include "Windows/fenetrebornes.h"
#include "ui_fenetrebornes.h"

FenetreBornes::FenetreBornes(Informations *info)
{
    informations = info;


    calculator = new ExprCalculator(false, informations->getFuncsList());

    ui = new Ui::FenetreBornes;
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
    connect(ui->orthonormal, SIGNAL(clicked(bool)), informations, SLOT(setOrthonormal(bool)));

    connect(ui->boutonAppliquer, SIGNAL(released()), this, SLOT(appliquer()));

    connect(info, SIGNAL(updateOccured()), this, SLOT(updateWidgets()));
}

void FenetreBornes::orthonormal(bool state)
{   
    ui->Ymax->setEnabled(!state);
    ui->Ymin->setEnabled(!state);
    ui->Ypas->setEnabled(!state);
}

void FenetreBornes::resetToStandardView()
{
    standardView();
    appliquer();
}

void FenetreBornes::standardView()
{
    ui->Xmax->setText("10");
    ui->Xmin->setText("-10");
    ui->Xpas->setText("1");

    ui->Ymax->setText("10");
    ui->Ymin->setText("-10");
    ui->Ypas->setText("1");
}

void FenetreBornes::appliquer()
{
    GraphRange range;
    bool ok = false;

    range.Xmax = calculator->calculateExpression(ui->Xmax->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Le nombre écrit en Xmax est faux"));
        messageBox->exec();
        return;
    }
    range.Xmin = calculator->calculateExpression(ui->Xmin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Le nombre écrit en Xmin est faux"));
        messageBox->exec();
        return;
    }
    range.Ymax = calculator->calculateExpression(ui->Ymax->text(),ok);
    if(ok == false)
    {
        messageBox->setText(tr("Le nombre écrit en Ymax est faux"));
        messageBox->exec();
        return;
    }
    range.Ymin = calculator->calculateExpression(ui->Ymin->text(), ok);
    if(ok == false)
    {
        messageBox->setText(tr("Le nombre écrit en Ymin est faux"));
        messageBox->exec();
        return;
    }
    range.Xscale = calculator->calculateExpression(ui->Xpas->text(), ok);
    if(ok == false || range.Xscale <= 0)
    {
        messageBox->setText(tr("Le nombre écrit dans le pas des X est faux"));
        messageBox->exec();
        return;
    }    

    if(ui->orthonormal->isChecked())
        ui->Ypas->setText(ui->Xpas->text());

    range.Yscale = calculator->calculateExpression(ui->Ypas->text(), ok);
    if(ok == false || range.Yscale <= 0)
    {
        messageBox->setText(tr("Le nombre écrit dans le pas des Y est faux"));
        messageBox->exec();
        return;
    }

    if(range.Xmin >= range.Xmax)
    {
        messageBox->setText(tr("Xmin doit être plus petit que Xmax"));
        messageBox->exec();
        return;
    }
    if(range.Ymin >= range.Ymax)
    {
        messageBox->setText(tr("Ymin doit être plus petit que Ymax"));
        messageBox->exec();
        return;
    }

    if(range.Ymax - range.Ymin < MIN_RANGE || range.Xmax - range.Xmin < MIN_RANGE)
    {
        messageBox->setText(tr("Les bornes sont trop petites pour être appliquées au graphique."));
        messageBox->exec();
        return;
    }

    informations->setOrthonormal(ui->orthonormal->isChecked());
    informations->setRange(range);
}

void FenetreBornes::updateWidgets()
 {
     GraphRange fenetre = informations->getRange();

     ui->Xmax->setText(QString::number(fenetre.Xmax));
     ui->Xmin->setText(QString::number(fenetre.Xmin));
     ui->Xpas->setText(QString::number(fenetre.Xscale));

     ui->Ymax->setText(QString::number(fenetre.Ymax));
     ui->Ymin->setText(QString::number(fenetre.Ymin));
     ui->Ypas->setText(QString::number(fenetre.Yscale));

     ui->orthonormal->setChecked(informations->isOrthonormal());
     orthonormal(informations->isOrthonormal());
 }


FenetreBornes::~FenetreBornes()
{
    delete ui;
    delete calculator;
}
