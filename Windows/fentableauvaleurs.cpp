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


#include "fentableauvaleurs.h"

FenTableauValeurs::FenTableauValeurs(Informations *info)
{
    informations = info;    

    setWindowTitle(tr("Tableaux de valeurs"));
    setWindowIcon(QIcon(":/icons/tableauDeValeurs.png"));
    QHBoxLayout *principalLayout = new QHBoxLayout();
    QPushButton *addButton = new QPushButton(tr("Ajouter"));
    addButton->setFixedSize(80,80);

    connect(addButton, SIGNAL(released()), this, SLOT(addValuesTable()));

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumSize(400,400);

    principalLayout->addWidget(scrollArea);
    principalLayout->addWidget(addButton);

    setLayout(principalLayout);

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

void FenTableauValeurs::addValuesTable()
{
    ValuesTable *table = new ValuesTable(informations);
    valuesTableList << table;

    connect(table, SIGNAL(remove(ValuesTable*)), this, SLOT(removeTable(ValuesTable*)));

    tablesLayout->addWidget(table);
}

void FenTableauValeurs::removeTable(ValuesTable *table)
{
    table->close();
    delete table;
}
