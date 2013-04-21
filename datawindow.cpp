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

#include "datawindow.h"
#include "ui_datawindow.h"

DataWindow::DataWindow(Informations *info)
{
    ui = new Ui::DataWindow;
    ui->setupUi(this);

    ui->actionsGroupBox->setFixedWidth(350);

    informations = info;
    selectorSide = COLUMN_SELECTION;

    columnSelector = new ColumnSelectorWidget(STARTING_COLUMN_COUNT);
    columnActionsWidget = new ColumnActionsWidget();
    rowSelector = new RowSelectorWidget(STARTING_ROW_COUNT);
    dataTable = new DataTable(info, STARTING_ROW_COUNT, STARTING_COLUMN_COUNT, ROW_HEIGHT, COLUMN_WIDTH);

    QHBoxLayout *columnSelectorLayout = new QHBoxLayout();
    columnSelectorLayout->setMargin(0);
    columnSelectorLayout->setSpacing(0);

    columnSelectorSpacer = new QWidget();
    columnSelector->setFixedHeight(COLUMN_SELECTOR_HEIGHT);

    columnSelectorLayout->addWidget(columnSelectorSpacer);
    columnSelectorLayout->addWidget(columnSelector);
    columnSelectorLayout->addStretch();

    ui->tableLayout->addLayout(columnSelectorLayout);

    QVBoxLayout *rowSelectorLayout = new QVBoxLayout();
    rowSelectorLayout->setMargin(0);
    rowSelectorLayout->setSpacing(0);

    rowSelectorSpacer = new QWidget();
    rowSelector->setFixedWidth(ROW_SELECTOR_WIDTH);

    rowSelectorLayout->addWidget(rowSelectorSpacer);
    rowSelectorLayout->addWidget(rowSelector);
    rowSelectorLayout->addStretch();

    QHBoxLayout *secondLayout = new QHBoxLayout();
    secondLayout->setMargin(0);
    secondLayout->setSpacing(0);

    secondLayout->addLayout(rowSelectorLayout);
    secondLayout->addWidget(dataTable);

    ui->tableLayout->addLayout(secondLayout);
    ui->tableLayout->addStretch();

    updateSpacerWidgets();

    connect(dataTable, SIGNAL(newPosCorrections()), this, SLOT(updateSpacerWidgets()));

    columnSelector->updateSelectorsPos();
    rowSelector->updateSelectorsPos();

    columnSelectorSpacer->setFixedWidth(dataTable->getVerticalHeaderSize().width());
    rowSelectorSpacer->setFixedHeight(dataTable->getHorizontalHeaderSize().height());

    ui->actionsLayout->addWidget(columnActionsWidget);

    connect(columnSelector, SIGNAL(newSelectorPos(bool,int)), columnActionsWidget, SLOT(setSelectorPos(bool,int)));
    connect(columnSelector, SIGNAL(askForSelector()), rowSelector, SLOT(askedForSelector()));
    connect(rowSelector, SIGNAL(askForSelector()), columnSelector, SLOT(askedForSelector()));

}

void DataWindow::selectorInColumnSelection()
{
    selectorSide = COLUMN_SELECTION;
}

void DataWindow::selectorInRowSelection()
{
    selectorSide = ROW_SELECTION;
}

void DataWindow::selectorPosChanged(bool inBetween, int index)
{
    if(selectorSide == ROW_SELECTION)
    {
        if(inBetween)
        {
            ui->actionsGroupBox->setTitle(tr("Actions entre deux colonnes :"));
        }
        else
        {
            ui->actionsGroupBox->setTitle(tr("Actions sur la colonne :"));
        }
    }
    else
    {
        if(inBetween)
        {
            ui->actionsGroupBox->setTitle(tr("Actions entre deux lignes :"));
        }
        else
        {
            ui->actionsGroupBox->setTitle(tr("Actions sur la ligne :"));
        }

    }
}

void DataWindow::updateSpacerWidgets()
{
    columnSelectorSpacer->setFixedWidth(ROW_SELECTOR_WIDTH + dataTable->getVerticalHeaderSize().width());
    rowSelectorSpacer->setFixedHeight(dataTable->getHorizontalHeaderSize().height());

    columnSelector->setFixedWidth(dataTable->getColumnCount()*COLUMN_WIDTH);
    rowSelector->setFixedHeight(dataTable->getRowCount()*ROW_HEIGHT);
}
