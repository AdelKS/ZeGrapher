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

#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include "informations.h"
#include "./datatable.h"
#include "./columnselectorwidget.h"
#include "./columnactionswidget.h"
#include "./rowselectorwidget.h"
#include "./rowactionswidget.h"

#define COLUMN_SELECTION true
#define ROW_SELECTION false

#define STARTING_COLUMN_COUNT 3
#define STARTING_ROW_COUNT 20
#define COLUMN_WIDTH 140
#define ROW_HEIGHT 30
#define ROW_SELECTOR_WIDTH 40
#define COLUMN_SELECTOR_HEIGHT 40

namespace Ui {
class DataWindow;
}

class DataWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DataWindow(Informations *info);

protected slots:
    void updateSelectorsSize();
    void selectorInColumnSelection();
    void selectorInRowSelection();
    void selectorPosChanged(bool inBetween, int index);

protected:

    Ui::DataWindow *ui;
    Informations *informations;
    QGroupBox *actionsGroupBox;
    QWidget *columnSelectorSpacer, *rowSelectorSpacer;
    DataTable *dataTable;
    ColumnSelectorWidget *columnSelector;
    ColumnActionsWidget *columnActionsWidget;
    RowSelectorWidget *rowSelector;
    RowActionsWidget *rowActionsWidget;
    bool selectorSide;
};

#endif // DATAWINDOW_H
