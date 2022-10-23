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

#ifndef DATATABLE_H
#define DATATABLE_H

#include <algorithm>

#include "Calculus/exprcalculator.h"

#define MIN_ROW_COUNT 10
#define MIN_COLUMN_COUNT 3

class DataTable : public QWidget
{
    Q_OBJECT
public:
    explicit DataTable(int rowCount, int columnCount, int rowHeight, int columnWidth);

    std::list<std::vector<double>>::const_iterator get_column_const_it(uint index) const;

    QSize getVerticalHeaderSize();
    QSize getHorizontalHeaderSize();

    QString getColumnName(int visualIndex);

    int getColumnCount();
    int getRowCount();

    const std::vector<double> &getSelectedColumn() const;

    QList<QStringList> getStringData();
    const std::vector<double> &getColumnConst(uint index) const;

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
    void checkVerticalHeaderNewWidth();

protected:
    void resizeColumns(int columnWidth);
    void resizeRows(int rowHeight);
    void addRow();
    void addColumn();
    void removeUnnecessaryColumns();
    void removeUnnecessaryRows();

    void swapDataRows(uint row1, uint row2);

    void updateTableFromData(bool refresh_all_columns = true,
                             bool refresh_all_rows = true,
                             uint row = 0, uint col = 0);

    auto column_it(uint index);

    bool eventFilter(QObject *obj, QEvent *event);

    bool rowCmpAscend(uint row_a, uint row_b);
    bool rowCmpDescend(uint row_a, uint row_b);

    std::vector<double> &getColumn(uint index);

    uint selectedCol;

    bool disableChecking;
    ExprCalculator calculator;
    TreeCreator treeCreator;
    int cellHeight, cellWidth, verticalHeaderWidth;
    QTableWidget *tableWidget;
    std::list<std::vector<double>> values; /* values[column][row] since there will be more rows than columns, column insertion is an implemented function in QList
                                    row insertion will be implemented */
    QStringList columnNames;
    QRegularExpression nameValidator;

};

#endif // DATATABLE_H
