/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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


#include "DataPlot/rowactionswidget.h"

RowActionsWidget::RowActionsWidget(int rownum)
{
    insertRow = new QPushButton(tr("Insérer ligne"));
    removeRow = new QPushButton(tr("Supprimer ligne"));

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(insertRow);
    mainLayout->addWidget(removeRow);

    insertRow->hide();

    setLayout(mainLayout);
    rowCount = rownum;

    connect(insertRow, SIGNAL(released()), this, SLOT(emitInsertRowSignal()));
    connect(removeRow, SIGNAL(released()), this, SLOT(emitRemoveRowSignal()));
}

void RowActionsWidget::setSelectorPos(bool betweenRows, int index)
{
    selectorPos.inbetween = betweenRows;
    selectorPos.index = index;

    if(betweenRows)
    {
        insertRow->show();
        removeRow->hide();
    }
    else if(rowCount > 10 && selectorPos.index != rowCount-1)
    {
        insertRow->hide();
        removeRow->show();
    }
    else
    {
        insertRow->hide();
        removeRow->hide();
    }
}

void RowActionsWidget::setRowCount(int count)
{
    rowCount = count;
    setSelectorPos(selectorPos.inbetween, selectorPos.index);
}

void RowActionsWidget::emitInsertRowSignal()
{    
    emit insertRowClicked(selectorPos.index);
}

void RowActionsWidget::emitRemoveRowSignal()
{    
    emit removeRowClicked(selectorPos.index);
}
