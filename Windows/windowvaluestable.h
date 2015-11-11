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



#ifndef WINDOWVALUESTABLE_H
#define WINDOWVALUESTABLE_H


#include "information.h"
#include "ValuesTable/valuestable.h"

namespace Ui {
    class WindowValuesTable;
}

class WindowValuesTable : public QWidget
{
    Q_OBJECT

public:
    explicit WindowValuesTable(Informations *info);

protected slots:
    void addValuesTable();
    void removeTable(ValuesTable *table);

protected:
    QHBoxLayout *tablesLayout;
    QList<ValuesTable*> valuesTableList;
    Informations *information;   
    QWidget *widget;

};

#endif // WINDOWVALUESTABLE_H
