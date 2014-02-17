/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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

#ifndef ABSTRACTTABLE_H
#define ABSTRACTTABLE_H

#include "Structures.h"

class AbstractTable : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractTable(QWidget *parent = 0);
    ~AbstractTable();
    
signals:
    void previous();

protected slots:

protected:
    QTableView *tableView;
    QStandardItemModel *model;
    QSpinBox *precision;
    QLabel *title;
    QWidget *k_parameter_widget;
    QLineEdit *k_value;


    QTimer *updateTimer;
    QFont boldFont;

    double k;
    
};

#endif // ABSTRACTTABLE_H
