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




#include "ValuesTable/abstracttable.h"

AbstractTable::AbstractTable(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(2);

    QHBoxLayout *previousButtonLayout = new QHBoxLayout;
    QPushButton *previousButton = new QPushButton(tr("Back"));
    QPushButton *exportButton = new QPushButton(tr("Export"));

    connect(previousButton, SIGNAL(released()), this, SIGNAL(previous()));
    connect(exportButton, SIGNAL(released()), this, SLOT(exportToCSV()));

    previousButtonLayout->addWidget(previousButton);
    previousButtonLayout->addStretch();
    previousButtonLayout->addWidget(exportButton);

    mainLayout->addLayout(previousButtonLayout);    

    //-----------------------------------------------------------------------

    QHBoxLayout *kWidgetLayout = new QHBoxLayout;
    kWidgetLayout->setMargin(0);

    QLabel *kText = new QLabel("k =");
    k_value = new QLineEdit;
    k_value->setMaximumHeight(25);

    kWidgetLayout->addWidget(kText);
    kWidgetLayout->addWidget(k_value);
    kWidgetLayout->addStretch();

    k_parameter_widget = new QWidget;
    k_parameter_widget->setLayout(kWidgetLayout);

    mainLayout->addWidget(k_parameter_widget);

    k_parameter_widget->hide();

    //--------------------------------------------------------------

    title = new QLabel;
    title->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(title);

    // -------------------------------------------------------------------

    model = new QStandardItemModel(this);

    tableView = new QTableView;
    tableView->horizontalHeader()->hide();
    tableView->verticalHeader()->hide();

    updateTimer = new QTimer(this);

    mainLayout->addWidget(tableView);

    //----------------------------------------------------------------

    QHBoxLayout *precisionLayout = new QHBoxLayout;
    QLabel *precisionText = new QLabel(tr("Precision:"));
    precision = new QSpinBox();
    precision->setSuffix(tr(" digits"));
    precision->setMinimum(2);
    precision->setMaximum(8);
    precision->setValue(4);

    precisionLayout->addStretch();
    precisionLayout->addWidget(precisionText);
    precisionLayout->addWidget(precision);
    precisionLayout->addStretch();

    mainLayout->addLayout(precisionLayout);

    //--------------------------------------------------------------------

    csvHandler = new CSVhandler(this);

    setLayout(mainLayout);

    boldFont.setBold(true);

}

AbstractTable::~AbstractTable()
{

}
