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

#include "datatable.h"

DataTable::DataTable(Informations *info, int rowCount, int columnCount, int rowHeight, int columnWidth)
{
    informations = info;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);   

    tableWidget = new QTableWidget(rowCount,columnCount);

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    resizeColumns(columnWidth);
    resizeRows(rowHeight);

    connect(tableWidget->verticalHeader(), SIGNAL(geometriesChanged()), this, SIGNAL(newPosCorrections()));
    connect(tableWidget->horizontalHeader(), SIGNAL(geometriesChanged()), this, SIGNAL(newPosCorrections()));

    mainLayout->addWidget(tableWidget);
    mainLayout->addStretch();
    setLayout(mainLayout);

    QStringList list;
    list << tr("Renommez moi!") << tr("Renommez moi!") << "";
    tableWidget->setHorizontalHeaderLabels(list);

}

void DataTable::resizeColumns(int columnWidth)
{
    cellWidth = columnWidth;

    for(int i = 0 ; i < tableWidget->columnCount(); i++)
    {
        tableWidget->setColumnWidth(i, columnWidth);
    }

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);
}

void DataTable::resizeRows(int rowHeight)
{
    cellHeight = rowHeight;

    for(int i = 0 ; i < tableWidget->rowCount(); i++)
    {
        tableWidget->setRowHeight(i, rowHeight);
    }

    tableWidget->setFixedHeight(tableWidget->rowCount() * cellHeight + tableWidget->horizontalHeader()->height() + 10);
}

QSize DataTable::getVerticalHeaderSize()
{
    return tableWidget->verticalHeader()->size();
}

QSize DataTable::getHorizontalHeaderSize()
{
    return tableWidget->horizontalHeader()->size();
}
