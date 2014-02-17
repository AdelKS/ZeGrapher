/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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


#include "fenetreoptions.h"
#include "ui_fenetreoptions.h"

FenetreOptions::FenetreOptions(Informations *info)
{
    informations = info;

    ui = new Ui::fenetreoptions;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/logoLogiciel.png"));

    parametres.couleurDuFond = QColor(Qt::white);
    parametres.couleurDesAxes = QColor(Qt::black);
    parametres.couleurQuadrillage = QColor(Qt::gray);


    connect(ui->appliquer, SIGNAL(clicked()), this, SLOT(appliquer()));
    connect(ui->couleurAxes, SIGNAL(clicked()), this, SLOT(changerCouleurAxes()));
    connect(ui->couleurFond, SIGNAL(clicked()), this, SLOT(changerCouleurFond()));
    connect(ui->couleurQuadrillage, SIGNAL(clicked()), this, SLOT(changerCouleurQuadrillage()));

}

void FenetreOptions::appliquer()
{
    if(parametres.couleurDesAxes == parametres.couleurDuFond)
        QMessageBox::warning(this, tr("Attention"), tr("Les couleurs du fond et des axes sont identiques"));
    else if(parametres.couleurDuFond == parametres.couleurQuadrillage)
        QMessageBox::warning(this, tr("Attention"), tr("Les couleurs du fond et du quadrillage sont identiques"));
    else
    {
        double dist = ui->distanceWidget->value();

        parametres.lissage = ui->lissage->isChecked();
        parametres.distanceEntrePoints = 0.5 * (10.25 - dist);
        parametres.epaisseurDesCourbes = ui->epaisseurWidget->value();
        parametres.numSize = ui->numSize->value();     

        informations->setOptions(parametres);
    }
}

void FenetreOptions::changerCouleurAxes()
{
    parametres.couleurDesAxes = QColorDialog::getColor(parametres.couleurDesAxes, this);
    ui->couleurAxes->setStyleSheet("background-color: rgb("+
                              QString::number(parametres.couleurDesAxes.red())+","+
                                   QString::number(parametres.couleurDesAxes.green())+","+
                                   QString::number(parametres.couleurDesAxes.blue())+")");
}

void FenetreOptions::changerCouleurFond()
{
    parametres.couleurDuFond = QColorDialog::getColor(parametres.couleurDuFond, this);
    ui->couleurFond->setStyleSheet("background-color: rgb("+
                              QString::number(parametres.couleurDuFond.red())+","+
                                   QString::number(parametres.couleurDuFond.green())+","+
                                   QString::number(parametres.couleurDuFond.blue())+")");
}

void FenetreOptions::changerCouleurQuadrillage()
{
    parametres.couleurQuadrillage = QColorDialog::getColor(parametres.couleurQuadrillage, this);
    ui->couleurQuadrillage->setStyleSheet("background-color: rgb("+
                              QString::number(parametres.couleurQuadrillage.red())+","+
                                   QString::number(parametres.couleurQuadrillage.green())+","+
                                   QString::number(parametres.couleurQuadrillage.blue())+")");
}

FenetreOptions::~FenetreOptions()
{
    delete ui;
}
