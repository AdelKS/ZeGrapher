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



#include "Windows/fenetreoptions.h"
#include "ui_fenetreoptions.h"

FenetreOptions::FenetreOptions(Informations *info)
{
    informations = info;

    ui = new Ui::fenetreoptions;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/logoLogiciel.png"));

    couleurAxes = new QColorButton(Qt::black);
    couleurAxes->setFixedSize(25,25);
    ui->axesColorLayout->addWidget(couleurAxes);

    couleurFond = new QColorButton(Qt::white);
    couleurFond->setFixedSize(25,25);
    ui->backgroundColorLayout->addWidget(couleurFond);

    couleurQuadrillage = new QColorButton(Qt::gray);
    couleurQuadrillage->setFixedSize(25,25);
    ui->gridColorLayout->addWidget(couleurQuadrillage);

    parametres.couleurDuFond = couleurFond->getCurrentColor();
    parametres.couleurDesAxes = couleurAxes->getCurrentColor();
    parametres.couleurQuadrillage = couleurQuadrillage->getCurrentColor();

    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(appliquer()));
    connect(ui->epaisseurWidget, SIGNAL(valueChanged(int)), this, SLOT(appliquer()));
    connect(ui->numSize, SIGNAL(valueChanged(int)), this, SLOT(appliquer()));
    connect(ui->lissage, SIGNAL(toggled(bool)), this, SLOT(appliquer()));
    connect(couleurAxes, SIGNAL(colorChanged(QColor)), this, SLOT(appliquer()));
    connect(couleurFond, SIGNAL(colorChanged(QColor)), this, SLOT(appliquer()));
    connect(couleurQuadrillage, SIGNAL(colorChanged(QColor)), this, SLOT(appliquer()));

    appliquer();

}

void FenetreOptions::appliquer()
{
    if(couleurAxes->getCurrentColor() == couleurFond->getCurrentColor())
        QMessageBox::warning(this, tr("Attention"), tr("Les couleurs du fond et des axes sont identiques"));
    else if(couleurFond->getCurrentColor() == couleurQuadrillage->getCurrentColor())
        QMessageBox::warning(this, tr("Attention"), tr("Les couleurs du fond et du quadrillage sont identiques"));
    else
    {
        double dist = ui->distanceWidget->value();

        parametres.lissage = ui->lissage->isChecked();
        parametres.distanceEntrePoints = pow(2, 2-dist/2);
        parametres.epaisseurDesCourbes = ui->epaisseurWidget->value();
        parametres.couleurDesAxes = couleurAxes->getCurrentColor();
        parametres.couleurDuFond = couleurFond->getCurrentColor();
        parametres.couleurQuadrillage = couleurQuadrillage->getCurrentColor();
        parametres.numSize = ui->numSize->value();     

        informations->setOptions(parametres);
    }
}

FenetreOptions::~FenetreOptions()
{
    delete ui;
}
