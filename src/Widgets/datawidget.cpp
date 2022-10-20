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



#include "Widgets/datawidget.h"
#include "ui_datawidget.h"

DataWidget::DataWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::DataWidget),
    colorButton(new QColorButton(information.getGraphSettings().estheticSettings.defaultColor)),
    userData(std::make_shared<UserData>()),
    dataWindow(new DataWindow(userData, this))
{
    ui->setupUi(this);
    ui->styleWidget->hide();

    ui->mainLayout->addWidget(colorButton);

    connect(ui->styleButton, SIGNAL(toggled(bool)), ui->styleWidget, SLOT(setVisible(bool)));

    ui->pointStyleCombo->setIconSize(ui->pointStyleCombo->size());
    ui->pointStyleCombo->addItem(QIcon(":/icons/rhombusPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/roundPoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/squarePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/trianglePoint.png"), "");
    ui->pointStyleCombo->addItem(QIcon(":/icons/crossPoint.png"), "");

    pointStyleMap << PointStyle::Rhombus << PointStyle::Disc << PointStyle::Square
                  << PointStyle::Triangle << PointStyle::Cross;

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

    QSettings settings;
    if(settings.contains("data_window/geometry"))
        dataWindow->setGeometry(settings.value("data_window/geometry").value<QRect>());

    connect(ui->showDataWindow, SIGNAL(released()), dataWindow, SLOT(show()));

    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect(ui->pointStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(newPointStyle(int)));
    connect(ui->linesStyleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(newLineStyle(int)));
    connect(ui->draw, SIGNAL(toggled(bool)), this, SLOT(changeDrawState(bool)));
    connect(ui->drawPoints, SIGNAL(toggled(bool)), this, SLOT(drawPoints(bool)));
    connect(ui->drawSegments, SIGNAL(toggled(bool)), this, SLOT(drawSegments(bool)));
}

DataWindow* DataWidget::getDataWindow()
{
    return dataWindow;
}

std::shared_ptr<const UserData> DataWidget::getUserData()
{
    return userData;
}

void DataWidget::drawSegments(bool draw)
{
    if(userData->style.drawLines != draw)
    {
        userData->style.drawLines = draw;
        information.emitDataUpdate();
    }
}

void DataWidget::drawPoints(bool draw)
{
    if(userData->style.drawPoints != draw)
    {
        userData->style.drawPoints = draw;
        information.emitDataUpdate();
    }
}

void DataWidget::setColor(QColor color)
{
    if(userData->style.color != color)
    {
        userData->style.color = color;
        information.emitDataUpdate();
    }
}

void DataWidget::newLineStyle(int index)
{
    if(userData->style.lineStyle != lineStyleMap[index])
    {
        userData->style.lineStyle = lineStyleMap[index];
        information.emitDataUpdate();
    }
}

void DataWidget::newPointStyle(int index)
{
    if(userData->style.pointStyle != pointStyleMap[index])
    {
        userData->style.pointStyle = pointStyleMap[index];
        information.emitDataUpdate();
    }

}

void DataWidget::changeDrawState(bool draw)
{
    userData->style.draw = draw;
    information.emitDataUpdate();
}

void DataWidget::closeDataWindow()
{
    dataWindow->close();
}

void DataWidget::emitRemoveSignal()
{
    emit removeMe(this);
}

DataWidget::~DataWidget()
{
    delete ui;
}
