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

    style.color = Qt::black;
    style.drawLines = false;
    style.drawPoints = true;
    style.lineStyle = Qt::SolidLine;
    style.pointStyle = Rhombus;
    style.draw = true;

    info->setDataStyle(num, style);

    informations = info;

    colorButton = new QColorButton();
    ui->mainLayout->addWidget(colorButton);

    connect(ui->styleButton, SIGNAL(toggled(bool)), ui->styleWidget, SLOT(setVisible(bool)));

    ui->pointStyleCombo->setIconSize(ui->pointStyleCombo->size());
    ui->pointStyleCombo->addItem(QIcon(":/icons/rhombusPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/roundPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/squarePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/trianglePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/crossPoint.png"), "");

    pointStyleMap << Rhombus << Disc << Square << Triangle << Cross;

    ui->linesStyleCombo->setIconSize(ui->linesStyleCombo->size());
    ui->linesStyleCombo->addItem(QIcon(":/icons/solidLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dashLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dotLine.png"), "");
    ui->linesStyleCombo->addItem(QIcon(":/icons/dashDotLine.png"), "");

    lineStyleMap << Qt::SolidLine << Qt::DashLine << Qt::DotLine << Qt::DashDotLine;

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setFlat(true);
    removeButton->setIconSize(QSize(25,25));
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveSignal()));
    ui->mainLayout->addWidget(removeButton);

    dataWindow = new DataWindow(info, num);
    connect(ui->showDataWindow, SIGNAL(released()), dataWindow, SLOT(show()));

    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect(ui->pointStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(newPointStyle(int)));
    connect(ui->linesStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(newLineStyle(int)));
    connect(ui->draw, SIGNAL(toggled(bool)), this, SLOT(changeDrawState(bool)));
    connect(ui->drawPoints, SIGNAL(toggled(bool)), this, SLOT(drawPoints(bool)));
    connect(ui->drawSegments, SIGNAL(toggled(bool)), this, SLOT(drawSegments(bool)));

     setWidgetNum(num);
}

void DataWidget::drawSegments(bool draw)
{
    if(style.drawLines != draw)
    {
        style.drawLines = draw;
        informations->setDataStyle(widgetNum, style);
    }
}

void DataWidget::drawPoints(bool draw)
{
    if(style.drawPoints != draw)
    {
        style.drawPoints = draw;
        informations->setDataStyle(widgetNum, style);
    }
}

void DataWidget::setColor(QColor color)
{
    if(style.color != color)
    {
        style.color = color;
        informations->setDataStyle(widgetNum, style);
    }
}

void DataWidget::newLineStyle(int index)
{
    if(style.lineStyle != lineStyleMap[index])
    {
        style.lineStyle = lineStyleMap[index];
        informations->setDataStyle(widgetNum, style);
    }
}

void DataWidget::newPointStyle(int index)
{
    if(style.pointStyle != pointStyleMap[index])
    {
        style.pointStyle = pointStyleMap[index];
        informations->setDataStyle(widgetNum, style);
    }

}

void DataWidget::changeDrawState(bool draw)
{
    style.draw = draw;
    informations->setDataStyle(widgetNum, style);
}

void DataWidget::closeDataWindow()
{
    dataWindow->close();
}

void DataWidget::setWidgetNum(int num)
{
    widgetNum = num;
    ui->nameLabel->setText("(" + tr("Données") + " " + QString::number(num+1) + ") :");
    dataWindow->changeIndex(num);
}

void DataWidget::emitRemoveSignal()
{
    emit removeMe(this);
}

DataWidget::~DataWidget()
{
    delete ui;
}
