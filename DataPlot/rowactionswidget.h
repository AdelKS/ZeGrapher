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


#ifndef ROWACTIONSWIDGET_H
#define ROWACTIONSWIDGET_H

#include "Structures.h"

class RowActionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RowActionsWidget(int rownum);

public slots:
    void setSelectorPos(bool betweenRows, int index);
    void setRowCount(int count);


signals:
    void removeRowClicked(int row);
    void insertRowClicked(int row);

protected slots:
    void emitRemoveRowSignal();
    void emitInsertRowSignal();

protected:

    int rowCount;
    SelectorPos selectorPos;
    QPushButton *removeRow, *insertRow;

    
};

#endif // ROWACTIONSWIDGET_H
