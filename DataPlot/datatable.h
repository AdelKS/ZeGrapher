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


#ifndef DATATABLE_H
#define DATATABLE_H

#include "Structures.h"
#include "informations.h"
#include "Calculus/exprcalculator.h"

#define MIN_ROW_COUNT 10
#define MIN_COLUMN_COUNT 3

class DataTable : public QWidget
{
    Q_OBJECT
public:
    explicit DataTable(Informations *info, int rowCount, int columnCount, int rowHeight, int columnWidth);
    ~DataTable();

    QSize getVerticalHeaderSize();
    QSize getHorizontalHeaderSize();

    QString getColumnName(int visualIndex);

    int getColumnCount();
    int getRowCount();

    QList<QStringList> getData();
    QList<QList<double> > &getValues();

    void fillColumnFromRange(int col, Range range);
    bool fillColumnFromExpr(int col, QString expr);

    void sortColumnSwapCells(int col, bool ascending);
    void sortColumnSwapRows(int column, bool ascending);

    int colVisualIndex(int logicalIndex);
    int colLogicalIndex(int visualIndex);

public slots:
    void insertRow(int index);
    void insertColumn(int index);

    void removeRow(int index);
    void removeColumn(int index);

    void addData(QList<QStringList> data);

signals:
    void newPosCorrections();   
    void newColumnCount(int count);
    void newRowCount(int count);
    void newColumnName(int visualIndex);
    void valEdited(int row, int column);
    void columnMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);

protected slots:
    void renameColumn(int index);
    void checkCell(QTableWidgetItem *item);    

protected:
    void resizeColumns(int columnWidth);
    void resizeRows(int rowHeight);
    void addRow();
    void addColumn();
    void removeUnnecessaryRowsColumns();



    bool disableChecking;
    ExprCalculator *calculator;
    TreeCreator *treeCreator;
    int cellHeight, cellWidth;
    Informations *informations;
    QTableWidget *tableWidget;
    QList<QList<double> > values; /* values[column][row] since there will be more rows than columns, column insertion is an implemented function in QList
                                    row insertion will be implemented */
    QStringList columnNames;
    QRegExp nameValidator;
    QPalette invalidPalette, validPalette;

    
};

#endif // DATATABLE_H
