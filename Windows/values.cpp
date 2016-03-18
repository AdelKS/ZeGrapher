/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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




#include "Windows/values.h"

Values::Values(Information *info)
{
    information = info;    

    setWindowTitle(tr("Values table"));
    setWindowIcon(QIcon(":/icons/valuesTable.png"));
    QHBoxLayout *mainLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton(tr("Add"));
    addButton->setFixedSize(80,80);

    connect(addButton, SIGNAL(released()), this, SLOT(addValuesTable()));

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumSize(400,400);

    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(addButton);

    setLayout(mainLayout);

    widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QHBoxLayout *widgetLayout = new QHBoxLayout;
    widgetLayout->setMargin(0);

    tablesLayout = new QHBoxLayout();
    tablesLayout->setSpacing(25);

    widgetLayout->addLayout(tablesLayout);
    widgetLayout->addStretch();

    widget->setLayout(widgetLayout);

    scrollArea->setWidget(widget);

    addValuesTable();
}

void Values::addValuesTable()
{
    ValuesTable *table = new ValuesTable(information);
    valuesTableList << table;

    connect(table, SIGNAL(remove(ValuesTable*)), this, SLOT(removeTable(ValuesTable*)));

    tablesLayout->addWidget(table);
}

void Values::removeTable(ValuesTable *table)
{
    table->close();
    delete table;
}
