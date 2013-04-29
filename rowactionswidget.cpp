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

#include "rowactionswidget.h"

RowActionsWidget::RowActionsWidget()
{
    insertRow = new QPushButton(tr("Insérer ligne"));
    removeRow = new QPushButton(tr("Supprimer ligne"));

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(insertRow);
    mainLayout->addWidget(removeRow);

    insertRow->hide();

    setLayout(mainLayout);

    connect(insertRow, SIGNAL(released()), this, SLOT(insertRowClicked()));
    connect(removeRow, SIGNAL(released()), this, SLOT(applyRemoval()));
}

void RowActionsWidget::setSelectorPos(bool betweenRows, int index)
{
    selectorPos.betweenColumns = betweenRows;
    selectorPos.index = index;

    if(betweenRows)
    {
        insertRow->show();
        removeRow->hide();
    }
    else
    {
        insertRow->hide();
        removeRow->show();
    }
}

void RowActionsWidget::applyRemoval()
{

}

void RowActionsWidget::insertRowClicked()
{

}
