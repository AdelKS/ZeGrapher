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


#include "abstracttable.h"

AbstractTable::AbstractTable(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *principalLayout = new QVBoxLayout;
    principalLayout->setMargin(2);

    QHBoxLayout *previousButtonLayout = new QHBoxLayout;
    QPushButton *previousButton = new QPushButton(tr("Retour"));

    connect(previousButton, SIGNAL(released()), this, SIGNAL(previous()));

    previousButtonLayout->addWidget(previousButton);
    previousButtonLayout->addStretch();

    principalLayout->addLayout(previousButtonLayout);    

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

    principalLayout->addWidget(k_parameter_widget);

    k_parameter_widget->hide();

    //--------------------------------------------------------------

    title = new QLabel;
    title->setAlignment(Qt::AlignHCenter);

    principalLayout->addWidget(title);

    // -------------------------------------------------------------------

    model = new QStandardItemModel(this);

    tableView = new QTableView;
    tableView->horizontalHeader()->hide();
    tableView->verticalHeader()->hide();

    updateTimer = new QTimer(this);

    principalLayout->addWidget(tableView);

    //----------------------------------------------------------------

    QHBoxLayout *precisionLayout = new QHBoxLayout;
    QLabel *precisionText = new QLabel(tr("Précision :"));
    precision = new QSpinBox;
    precision->setMinimum(2);
    precision->setMaximum(8);
    precision->setValue(4);

    precisionLayout->addStretch();
    precisionLayout->addWidget(precisionText);
    precisionLayout->addWidget(precision);
    precisionLayout->addStretch();

    principalLayout->addLayout(precisionLayout);

    //--------------------------------------------------------------------

    setLayout(principalLayout);

    boldFont.setBold(true);

}

AbstractTable::~AbstractTable()
{

}
