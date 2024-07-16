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



#ifndef DATAWINDOW_H
#define DATAWINDOW_H

#include "./datatable.h"
#include "./columnselectorwidget.h"
#include "./columnactionswidget.h"
#include "./rowselectorwidget.h"
#include "./rowactionswidget.h"
#include "./csvhandler.h"
#include "./modelwidget.h"

#define WIDGET_ANIMATION_TIME 250

#define COLUMN_SELECTION true
#define ROW_SELECTION false

#define STARTING_XPIN_INDEX 0
#define STARTING_YPIN_INDEX 1
#define STARTING_SELECTOR_INDEX 2

#define STARTING_COLUMN_COUNT 3
#define STARTING_ROW_COUNT 20
#define COLUMN_WIDTH 170
#define ROW_HEIGHT 30
#define ROW_SELECTOR_WIDTH 40
#define COLUMN_SELECTOR_HEIGHT 40

enum RetractableWidgetState { WIDGET_OPENED, WIDGET_RETRACTED };

namespace Ui {
class DataWindow;
}

class DataWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DataWindow(const std::shared_ptr<UserData> &userData, QWidget *parent = 0);
    RetractableWidgetState getRetractableWidgetState();

    ~DataWindow();

signals:
    void showHelpWindow();

protected slots:
    void updateSelectorsSize();
    void selectorInColumnSelection();
    void selectorInRowSelection();
    void selectorPosChanged(bool inBetween, int index);
    void openData();
    void saveData();
    void cellValChanged(int row, int col);
    void dataChanged();
    void remakeDataList();
    void addModel();
    void removeModelWidget(ModelWidget* w);
    void coordinateSystemChanged(bool polar);
    void columnNameChanged(int index);
    void columnMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void animationFinished();
    void startAnimation();
    void displayHelp();

protected:

    int xindex, yindex;
    Ui::DataWindow *ui;
    QGroupBox *actionsGroupBox;
    QWidget *columnSelectorSpacer, *rowSelectorSpacer;
    DataTable *dataTable;
    ColumnSelectorWidget *columnSelector;
    ColumnActionsWidget *columnActionsWidget;
    RowSelectorWidget *rowSelector;
    RowActionsWidget *rowActionsWidget;
    CSVhandler *csvHandler;
    bool selectorSide;
    RetractableWidgetState widgetState;
    int animation_width;
    QSplitter *widgetSplitter;

    QPropertyAnimation *windowCloseAnimation, *windowOpenAnimation, *widgetCloseAnimation, *widgetOpenAnimation;
    QParallelAnimationGroup *openAnimation, *closeAnimation;
    std::weak_ptr<UserData> modelData;
    QList<ModelWidget*> modelWidgets;
};

#endif // DATAWINDOW_H
