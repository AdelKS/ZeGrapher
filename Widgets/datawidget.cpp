/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
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

#include "Widgets/datawidget.h"
#include "ui_datawidget.h"

DataWidget::DataWidget(int num, Informations *info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataWidget)
{
    ui->setupUi(this);
    ui->styleWidget->hide();

    setWidgetNum(num);

    colorButton = new QColorButton();
    ui->mainLayout->addWidget(colorButton);

    connect(ui->styleButton, SIGNAL(toggled(bool)), ui->styleWidget, SLOT(setShown(bool)));

    ui->pointStyleCombo->setIconSize(ui->pointStyleCombo->size());
    ui->pointStyleCombo->addItem(QIcon(":/icons/rhombusPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/roundPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/squarePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/trianglePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/crossPoint.png"), "");

    ui->linesStyleCombo->setIconSize(ui->linesStyleCombo->size());
    ui->linesStyleCombo->addItem(QIcon(":/icons/solidLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dashLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dotLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dashDotLine.png"), "");

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setFlat(true);
    removeButton->setIconSize(QSize(25,25));
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveSignal()));
    ui->mainLayout->addWidget(removeButton);

    dataWindow = new DataWindow(info);
    connect(ui->showDataWindow, SIGNAL(released()), dataWindow, SLOT(show()));
}

void DataWidget::setWidgetNum(int num)
{
    ui->nameLabel->setText("(" + tr("Données") + " " + QString::number(num) + ") :");
}

void DataWidget::emitRemoveSignal()
{
    emit removeMe(this);
}

DataWidget::~DataWidget()
{
    delete ui;
}
