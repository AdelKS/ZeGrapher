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



#include "DataPlot/columnactionswidget.h"
#include "ui_sortoptions.h"
#include "ui_confirmdelete.h"
#include "ui_filloptions.h"
#include "ui_startingactions.h"

ColumnActionsWidget::ColumnActionsWidget(DataTable *table, int columnnum):
    calculator(false, information.getFuncsList()),
    signalMapper(new QSignalMapper(this)),
    lineEditsMapper(new QSignalMapper(this)),
    startingActions(new QWidget()),
    fillOptions(new QWidget()),
    sortOptions(new QWidget()),
    confirmDelete(new QWidget()),
    insertColumn(new QWidget()),
    dataTable(table),
    selectorPos({false, 2}),
    confirmDeleteUi(new Ui::ConfirmDelete),
    fillOptionsUi(new Ui::FillOptions),
    sortOptionsUi(new Ui::SortOptions),
    startingActionsUi(new Ui::StartingActions)
{
    connect(signalMapper, SIGNAL(mappedObject(QObject*)), this, SLOT(showNextWidget(QObject*)));

    startingActionsUi->setupUi(startingActions);
    startingActionsUi->remove->hide();

    shownWidgets << startingActions;


    fillOptionsUi->setupUi(fillOptions);

    connect(fillOptionsUi->previous, SIGNAL(released()), this, SLOT(showPreviousWidget()));

    connect(fillOptionsUi->start, SIGNAL(returnPressed()), this, SLOT(applyFill()));
    connect(fillOptionsUi->step, SIGNAL(returnPressed()), this, SLOT(applyFill()));
    connect(fillOptionsUi->end, SIGNAL(returnPressed()), this, SLOT(applyFill()));
    connect(fillOptionsUi->expression, SIGNAL(returnPressed()), this, SLOT(applyFill()));
    connect(fillOptionsUi->apply, SIGNAL(released()), this, SLOT(applyFill()));

    connect(lineEditsMapper, SIGNAL(mappedObject(QObject*)), this, SLOT(resetPalette(QObject*)));

    connect(fillOptionsUi->start, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->start, fillOptionsUi->start);

    connect(fillOptionsUi->end, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->end, fillOptionsUi->end);

    connect(fillOptionsUi->step, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->step, fillOptionsUi->step);

    connect(fillOptionsUi->expression, SIGNAL(textChanged(QString)), lineEditsMapper, SLOT(map()));
    lineEditsMapper->setMapping(fillOptionsUi->expression, fillOptionsUi->expression);

    sortOptionsUi->setupUi(sortOptions);
    connect(sortOptionsUi->previous, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(sortOptionsUi->apply, SIGNAL(released()), this, SLOT(applySort()));


    confirmDeleteUi->setupUi(confirmDelete);
    confirmDeleteUi->textPromptLabel->setText(tr("Confirm column deletion?"));
    connect(confirmDeleteUi->no, SIGNAL(released()), this, SLOT(showPreviousWidget()));
    connect(confirmDeleteUi->yes, SIGNAL(released()), this, SLOT(emitRemoveColumnSignal()));


    QHBoxLayout *insertColumnLayout = new QHBoxLayout();
    QPushButton *insertButton = new QPushButton(tr("Insert column"));

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
    mainLayout->setContentsMargins(3, 3, 3, 3);
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

    setColumnCount(columnnum);
}

void ColumnActionsWidget::resetPalette(QObject *obj)
{
    QWidget* lineEdit = qobject_cast<QLineEdit*>(obj);
    if (lineEdit)
        lineEdit->setPalette(QPalette());
    else qWarning() << "problem in ColumnActionsWidget::resetPalette(QObject *obj) \n";
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

void ColumnActionsWidget::showNextWidget(QObject *obj)
{
    shownWidgets.last()->hide();
    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (widget)
    {
        widget->show();
        shownWidgets << widget;
    }
    else qWarning() << "problem in ColumnActionsWidget::showNextWidget(QObject *obj) \n";

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
        bool goodEntry = true;

        auto compute_check = [](double &val, QLineEdit* widget)
        {
            auto res = zc::Expression(widget->text().toStdString()).evaluate(information.getMathWorld());
            if(res)
            {
                val = *res;
                widget->setPalette(information.getValidSyntaxPalette());
                return true;
            }
            else
            {
                widget->setPalette(information.getInvalidSyntaxPalette());
                return false;
            }
        };

        goodEntry &= compute_check(range.start, fillOptionsUi->start);
        goodEntry &= compute_check(range.end, fillOptionsUi->end);
        goodEntry &= compute_check(range.step, fillOptionsUi->step);

        if(goodEntry)
        {
            dataTable->fillColumnFromRange(selectorPos.index, range);
            shownWidgets.last()->hide();
            shownWidgets.clear();
            startingActions->show();
            shownWidgets << startingActions;

            resetFillForms();
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

            resetFillForms();
        }
        else fillOptionsUi->expression->setPalette(information.getInvalidSyntaxPalette());
    }
}

void ColumnActionsWidget::resetFillForms()
{
    fillOptionsUi->start->clear();
    fillOptionsUi->step->clear();
    fillOptionsUi->end->clear();
    fillOptionsUi->expression->clear();

    fillOptionsUi->start->setPalette(QPalette());
    fillOptionsUi->step->setPalette(QPalette());
    fillOptionsUi->end->setPalette(QPalette());
    fillOptionsUi->expression->setPalette(QPalette());
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

ColumnActionsWidget::~ColumnActionsWidget()
{
    delete confirmDeleteUi;
    delete fillOptionsUi;
    delete sortOptionsUi;
    delete startingActionsUi;
}
