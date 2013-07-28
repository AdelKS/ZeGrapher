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

#ifndef DATATABLE_H
#define DATATABLE_H

#include "Structures.h"
#include "informations.h"

class DataTable : public QWidget
{
    Q_OBJECT
public:
    explicit DataTable(Informations *info, int rowCount, int columnCount, int rowHeight, int columnWidth);
    QSize getVerticalHeaderSize();
    QSize getHorizontalHeaderSize();

    int getColumnCount();
    int getRowCount();

signals:
    void newPosCorrections();
    void newColumnNames(QStringList names);

protected slots:
    void renameColumn(int index);

protected:
    void resizeColumns(int columnWidth);
    void resizeRows(int rowHeight);

    int cellHeight, cellWidth;
    Informations *informations;
    QTableWidget *tableWidget;
    QList<QList<double> > values;
    QStringList columnNames;
    QRegExp nameValidator;

    
};

#endif // DATATABLE_H
