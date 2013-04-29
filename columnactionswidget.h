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

#ifndef COLUMNACTIONSWIDGET_H
#define COLUMNACTIONSWIDGET_H

#include "Structures.h"

namespace Ui {
class FillOptions;
class SortOptions;
class ConfirmDelete;
class StartingActions;
}

class ColumnActionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColumnActionsWidget();

public slots:
    void setSelectorPos(bool betweenColumns, int index);
    
protected slots:
    void applyFill();
    void applySort();
    void applyRemoval();
    void insertColumnClicked();

    void showNextWidget(QWidget*);
    void showPreviousWidget();

protected:
    QSignalMapper *signalMapper;
    QWidget *startingActions, *fillOptions, *sortOptions, *confirmDelete, *insertColumn;
    QList<QWidget*> shownWidgets;

    SelectorPos selectorPos;

    Ui::ConfirmDelete *confirmDeleteUi;
    Ui::FillOptions *fillOptionsUi;
    Ui::SortOptions *sortOptionsUi;
    Ui::StartingActions *startingActionsUi;

};

#endif // COLUMNACTIONSWIDGET_H
