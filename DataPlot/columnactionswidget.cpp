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

#include "columnactionswidget.h"
#include "ui_sortoptions.h"
#include "ui_confirmdelete.h"
#include "ui_filloptions.h"
#include "ui_startingactions.h"

ColumnActionsWidget::ColumnActionsWidget(DataTable *table)
{
    dataTable = table;

    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(showNextWidget(QWidget*)));

    startingActions = new QWidget();
    startingActionsUi = new Ui::StartingActions;
    startingActionsUi->setupUi(startingActions);

    shownWidgets << startingActions;

    fillOptions = new QWidget();
    fillOptionsUi = new Ui::FillOptions;
    fillOptionsUi->setupUi(fillOptions);
    connect(fillOptionsUi->previous, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(fillOptionsUi->apply, SIGNAL(released()), this, SLOT(applyFill()));

    sortOptions = new QWidget();
    sortOptionsUi = new Ui::SortOptions;
    sortOptionsUi->setupUi(sortOptions);
    connect(sortOptionsUi->previous, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(sortOptionsUi->apply, SIGNAL(released()), this, SLOT(applySort()));

    confirmDelete = new QWidget();
    confirmDeleteUi = new Ui::ConfirmDelete;
    confirmDeleteUi->setupUi(confirmDelete);
    confirmDeleteUi->textPromptLabel->setText(tr("Etes vous sûr de supprimer cette colonne ?"));
    connect(confirmDeleteUi->no, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(confirmDeleteUi->yes, SIGNAL(released()), this, SLOT(emitRemoveColumnSignal()));

    insertColumn = new QWidget();
    QHBoxLayout *insertColumnLayout = new QHBoxLayout();
    QPushButton *insertButton = new QPushButton(tr("Insérer colonne"));

    insertColumnLayout->addWidget(insertButton);

    connect(insertButton, SIGNAL(released()), this, SLOT(emitInsertColumnSignal()));
    insertColumn->setLayout(insertColumnLayout);


    connect(startingActionsUi->fill, SIGNAL(released()), signalMapper, SLOT(map()));
    signalMapper->setMapping(startingActionsUi->fill, fillOptions);

    connect(startingActionsUi->sort, SIGNAL(released()), signalMapper, SLOT(map()));
    signalMapper->setMapping(startingActionsUi->sort, sortOptions);

    connect(startingActionsUi->remove, SIGNAL(released()), signalMapper, SLOT(map()));
    signalMapper->setMapping(startingActionsUi->remove, confirmDelete);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startingActions);
    mainLayout->addWidget(fillOptions);
    mainLayout->addWidget(sortOptions);
    mainLayout->addWidget(confirmDelete);
    mainLayout->addWidget(insertColumn);

    setLayout(mainLayout);

    fillOptions->hide();
    sortOptions->hide();
    confirmDelete->hide();
    insertColumn->hide();
}

void ColumnActionsWidget::setSelectorPos(bool betweenColumns, int index)
{
    if(selectorPos.betweenColumns != betweenColumns || selectorPos.index != index)
    {
        selectorPos.betweenColumns = betweenColumns;
        selectorPos.index = index;

        shownWidgets.last()->hide();
        shownWidgets.clear();

        if(selectorPos.betweenColumns)
        {            
            insertColumn->show();
            shownWidgets << insertColumn;
        }
        else
        {
            startingActions->show();
            shownWidgets << startingActions;
        }

    }
}

void ColumnActionsWidget::showNextWidget(QWidget *widget)
{
    shownWidgets.last()->hide();
    widget->show();
    shownWidgets << widget;
}

void ColumnActionsWidget::showPreviousWidget()
{
    shownWidgets.last()->hide();
    shownWidgets.removeLast();
    shownWidgets.last()->show();
}

void ColumnActionsWidget::applyFill()
{

}

void ColumnActionsWidget::applySort()
{

}

void ColumnActionsWidget::emitInsertColumnSignal()
{
    emit insertColumnClicked(selectorPos.index);
}

void ColumnActionsWidget::emitRemoveColumnSignal()
{
    emit removeColumnClicked(selectorPos.index);
    showPreviousWidget();
}
