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

#include "DataPlot/columnactionswidget.h"
#include "ui_sortoptions.h"
#include "ui_confirmdelete.h"
#include "ui_filloptions.h"
#include "ui_startingactions.h"

ColumnActionsWidget::ColumnActionsWidget(DataTable *table, Informations *info, int columnnum)
{
    informations = info;
    calculator = new ExprCalculator(false, info->getFuncsList());
    dataTable = table;
    columnCount = columnnum;

    selectorPos.index = 2;
    selectorPos.inbetween = false;

    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);

    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(showNextWidget(QWidget*)));   

    startingActions = new QWidget();
    startingActionsUi = new Ui::StartingActions;
    startingActionsUi->setupUi(startingActions);
    startingActionsUi->remove->hide();

    startingActions->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    shownWidgets << startingActions;

    fillOptions = new QWidget();
    fillOptionsUi = new Ui::FillOptions;
    fillOptionsUi->setupUi(fillOptions);
    connect(fillOptionsUi->previous, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(fillOptionsUi->apply, SIGNAL(released()), this, SLOT(applyFill()));

    fillOptions->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    lineEditsMapper = new QSignalMapper(this);
    connect(lineEditsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(resetPalette(QWidget*)));

    connect(fillOptionsUi->start, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->start, fillOptionsUi->start);

    connect(fillOptionsUi->end, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->end, fillOptionsUi->end);

    connect(fillOptionsUi->step, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->step, fillOptionsUi->step);

    connect(fillOptionsUi->expression, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->expression, fillOptionsUi->expression);

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
    mainLayout->setMargin(3);
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

void ColumnActionsWidget::resetPalette(QWidget *widget)
{
    QLineEdit *lineEdit = (QLineEdit*)widget;
    lineEdit->setPalette(neutralPalette);
}

void ColumnActionsWidget::setSelectorPos(bool betweenColumns, int index)
{
    if(selectorPos.inbetween != betweenColumns || selectorPos.index != index)
    {
        selectorPos.inbetween = betweenColumns;
        selectorPos.index = index;

        shownWidgets.last()->hide();
        shownWidgets.clear();

        if(selectorPos.inbetween)
        {            
            insertColumn->show();
            shownWidgets << insertColumn;
        }
        else if (columnCount > 3)
        {
            startingActionsUi->remove->show();
            startingActions->show();
            shownWidgets << startingActions;
        }
        else
        {
            startingActionsUi->remove->hide();
            startingActions->show();
            shownWidgets << startingActions;
        }

    }
}

void ColumnActionsWidget::setColumnCount(int count)
{
    columnCount = count;
    setSelectorPos(selectorPos.inbetween, selectorPos.index);
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
    if(fillOptionsUi->predefined->isChecked())
    {
        Range range;
        bool goodEntry = true, ok = false;

        range.start = calculator->calculateExpression(fillOptionsUi->start->text(), ok);
        if(ok)
            fillOptionsUi->start->setPalette(validPalette);
        else fillOptionsUi->start->setPalette(invalidPalette);
        goodEntry &= ok;

        range.end = calculator->calculateExpression(fillOptionsUi->end->text(), ok);
        if(ok)
            fillOptionsUi->end->setPalette(validPalette);
        else fillOptionsUi->end->setPalette(invalidPalette);
        goodEntry &= ok;

        range.step = calculator->calculateExpression(fillOptionsUi->step->text(), ok);
        if(ok)
            fillOptionsUi->step->setPalette(validPalette);
        else fillOptionsUi->step->setPalette(invalidPalette);
        goodEntry &= ok;

        if(goodEntry)
        {
            dataTable->fillColumnFromRange(selectorPos.index, range);
            shownWidgets.last()->hide();
            shownWidgets.clear();
            startingActions->show();
            shownWidgets << startingActions;

            fillOptionsUi->start->clear();
            fillOptionsUi->step->clear();
            fillOptionsUi->end->clear();
            fillOptionsUi->expression->clear();
        }
    }
    else
    {
        if(dataTable->fillColumnFromExpr(selectorPos.index, fillOptionsUi->expression->text()))
        {
            shownWidgets.last()->hide();
            shownWidgets.clear();
            startingActions->show();
            shownWidgets << startingActions;

            fillOptionsUi->start->clear();
            fillOptionsUi->step->clear();
            fillOptionsUi->end->clear();
            fillOptionsUi->expression->clear();
        }
        else fillOptionsUi->expression->setPalette(invalidPalette);
    }
}

void ColumnActionsWidget::applySort()
{
    if(sortOptionsUi->swapCells->isChecked())
        dataTable->sortColumnSwapCells(selectorPos.index, sortOptionsUi->ascending->isChecked());
    else dataTable->sortColumnSwapRows(selectorPos.index, sortOptionsUi->ascending->isChecked());

    shownWidgets.last()->hide();
    shownWidgets.clear();
    startingActions->show();
    shownWidgets << startingActions;
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
