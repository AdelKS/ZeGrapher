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




#ifndef VALUESTABLE_H
#define VALUESTABLE_H

#include "structures.h"
#include "information.h"
#include "valuestableconf.h"
#include "seqtable.h"
#include "functable.h"
#include "pareqtable.h"


class ValuesTable : public QWidget
{
    Q_OBJECT
public:
    explicit ValuesTable(Information *info, QWidget *parent = 0);
    ~ValuesTable();
    
signals:
    void remove(ValuesTable *table);

protected slots:   
    void apply(ValuesTableParameters parameters);
    void emitRemoveSignal();
    void previous();

protected:
    ValuesTableConf *confWidget;
    Information *infoClass;   
    SeqTable *seqTable;
    FuncTable *funcTable;
    ParEqTable *parEqTable;
    QVBoxLayout *containerLayout;
    
};

#endif // VALUESTABLE_H
