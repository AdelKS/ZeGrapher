/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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




#ifndef VALUES_H
#define VALUES_H


#include "information.h"
#include "ValuesTable/valuestable.h"

namespace Ui {
    class Values;
}

class Values : public QWidget
{
    Q_OBJECT

public:
    explicit Values(Information *info);

protected slots:
    void addValuesTable();
    void removeTable(ValuesTable *table);

protected:
    QHBoxLayout *tablesLayout;
    QList<ValuesTable*> valuesTableList;
    Information *information;   
    QWidget *widget;

};

#endif // ValuesTable_H
