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

#include "DataPlot/datatable.h"

DataTable::DataTable(Information *info, int rowCount, int columnCount,
                     int rowHeight, int columnWidth):
    selectedCol(0),
    calculator(false, info->getFuncsList()),
    treeCreator(ObjectType::DATA_TABLE_EXPR),
    information(info),
    tableWidget(new QTableWidget(rowCount,columnCount))
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setMinimumSize(0,0);

    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    tableWidget->horizontalHeader()->setSectionsMovable(true);

    tableWidget->horizontalHeader()->setFixedHeight(25);

    resizeColumns(columnWidth);
    resizeRows(rowHeight);

    disableChecking = true;

    for(int col = 0 ; col < columnCount ; col++)
    {
        auto &column = this->values.emplace_back();
        column.reserve(rowCount);
        for(int row = 0 ; row < rowCount ; row++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(" ");
            tableWidget->setItem(row, col, item);
            column.push_back(nan("")) ;
        }
    }

    backgroundColor = palette().base().color();
    textColor = palette().text().color();

    disableChecking = false;

    connect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(checkCell(QTableWidgetItem*)));

    connect(tableWidget->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(renameColumn(int)));
    connect(tableWidget->horizontalHeader(), SIGNAL(sectionMoved(int,int,int)), this, SIGNAL(columnMoved(int, int, int)));

    mainLayout->addWidget(tableWidget);
    mainLayout->addStretch();
    setLayout(mainLayout);

    for(int i = 0 ; i < columnCount ; i++) { columnNames << tr("Rename me!") ; }
    tableWidget->setHorizontalHeaderLabels(columnNames);

    tableWidget->installEventFilter(this);

    nameValidator.setPattern("^([a-z]|[A-Z])+(_([a-z]|[A-Z])+)*$");

}

void DataTable::checkVerticalHeaderNewWidth()
{
    if(verticalHeaderWidth != tableWidget->verticalHeader()->width())
    {
        verticalHeaderWidth = tableWidget->verticalHeader()->width();
        emit newPosCorrections();
    }
}

bool DataTable::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->key() == Qt::Key_Return && tableWidget->currentItem() != nullptr)
        {
            QTableWidgetItem *item = tableWidget->currentItem();
            tableWidget->setCurrentItem(tableWidget->item(item->row()+1,item->column()));
            return true;
        }
        else if( (keyEvent->key() == Qt::Key_Delete || keyEvent->key() == Qt::Key_Backspace) && !tableWidget->selectedItems().empty())
        {
            for(auto &item : tableWidget->selectedItems())
            {
                item->setText("");
            }
            removeUnnecessaryRows();
            removeUnnecessaryColumns();

            return true;
        }
        return false;

    }
    return false;
}

QString DataTable::getColumnName(int visualIndex)
{
    return tableWidget->horizontalHeaderItem(tableWidget->horizontalHeader()->logicalIndex(visualIndex))->text();
}

int doubleCompareAscend(double a, double b)
{
    if(std::isnan(b))
        return -1;
    else return a<b;
}

int doubleCompareDescend(double a, double b)
{
    if(std::isnan(b))
        return 1;
    else return a>b;
}

QList<QStringList> DataTable::getStringData()
{
    QList<QStringList> data;

    QStringList columnNamesVisualOrder;
    for(int i = 0 ; i < tableWidget->horizontalHeader()->count() ; i++)
        columnNamesVisualOrder << tableWidget->horizontalHeaderItem(tableWidget->horizontalHeader()->logicalIndex(i))->text();

    data << columnNamesVisualOrder;

    for(int i = 0 ; i < tableWidget->rowCount(); i++)
    {
        data << QStringList();
        for(int j = 0 ; j < tableWidget->columnCount(); j++)
            data[i+1] << tableWidget->item(i,tableWidget->horizontalHeader()->logicalIndex(j))->text();
    }

    return data;
}

void DataTable::removeUnnecessaryColumns()
{
    //removing unnecessary columns but not the last one

    bool unnecessary = false;

    int col = 0;

    while(col < tableWidget->columnCount()-1 && tableWidget->columnCount() > MIN_COLUMN_COUNT)
    {
        if(columnNames[col] != tr("Rename me!"))
        {
            col++;
            continue;
        }

        unnecessary = true;

        for(int row = 0 ; row < tableWidget->rowCount() && unnecessary; row++)
            unnecessary = tableWidget->item(row, col)->text().isEmpty() || tableWidget->item(row, col)->text() == " ";

        if(unnecessary && tableWidget->columnCount() > MIN_COLUMN_COUNT)
            removeColumn(col);
        else col++;
    }
}

void DataTable::removeUnnecessaryRows()
{
    //removing rows, but not the last one

    int row = 0;
    bool unnecessary = false;

    while(row < tableWidget->rowCount()-1 && tableWidget->rowCount() > MIN_ROW_COUNT)
    {
        unnecessary = true;

        for(int col = 0 ; col < tableWidget->columnCount() && unnecessary; col++)
            unnecessary = tableWidget->item(row, col)->text().isEmpty() || tableWidget->item(row, col)->text() == " ";

        if(unnecessary && tableWidget->rowCount() > MIN_ROW_COUNT)
            removeRow(row);
        else row++;
    }
}

auto DataTable::column_it(uint index)
{
    auto it = values.begin();
    std::advance(it, index);
    return it;
}

std::list<std::vector<double>>::const_iterator DataTable::get_column_const_it(uint index) const
{
    auto it = values.cbegin();
    std::advance(it, index);
    return it;
}

const std::vector<double> &DataTable::getColumnConst(uint index) const
{
    return *get_column_const_it(index);
}

std::vector<double> &DataTable::getColumn(uint index)
{
    return *column_it(index);
}

void DataTable::addData(QList<QStringList> data)
{
    //the first line of the CSV file must contain column names, matching the validator, or left blank

    int indexOfLastInsertedColumn = 0;

    if(!data.isEmpty())
    {
        for(int column = 0 ; column < data[0].size(); column++)
        {
            if(column == indexOfLastInsertedColumn)
            {
                insertColumn(column);
                indexOfLastInsertedColumn++;
            }

            if(nameValidator.match(data[0][column]).hasMatch())
                columnNames[column] = data[0][column];
        }
    }

    tableWidget->setHorizontalHeaderLabels(columnNames);

    for(int row = 1 ; row < data.size(); row++)
    {
        if(tableWidget->rowCount() == row)
            addRow();

        for(int column = 0 ; column < data[row].size(); column++)
        {
            if(column == indexOfLastInsertedColumn)
            {
                insertColumn(column);
                indexOfLastInsertedColumn++;
            }

            tableWidget->item(row-1, column)->setText(data[row][column]);
        }
    }

    removeUnnecessaryRows();
    removeUnnecessaryColumns();
}

void DataTable::sortColumnSwapCells(int col, bool ascending)
{
    col = tableWidget->horizontalHeader()->logicalIndex(col);

    if(ascending)
        std::sort(column_it(col)->begin(), column_it(col)->end(), doubleCompareAscend);
    else std::sort(column_it(col)->begin(), column_it(col)->end(), doubleCompareDescend);

    updateTableFromData(false, true, 0, col);

    emit valEdited(0, col);
}

void DataTable::updateTableFromData(bool refresh_all_columns,
                         bool refresh_all_rows, uint row, uint col)
{
    disableChecking = true;

    uint row_max = row+1;
    if(refresh_all_rows)
    {
        row_max = tableWidget->rowCount();
        row = 0;
    }

    uint col_max = col+1;
    if(refresh_all_columns)
    {
        col_max = tableWidget->columnCount();
        col = 0;
    }

    for(uint row_index = row ; row_index < row_max; row_index++)
    {
        for(uint col_index = col ; col_index < col_max; col_index++)
        {
            if(std::isnan(getColumnConst(col_index)[row_index]))
            {
                tableWidget->item(row,col)->setText("");
                tableWidget->item(row,col)->setBackground(QBrush(backgroundColor));
                tableWidget->item(row,col)->setForeground(QBrush(textColor));
            }
            else
            {
                tableWidget->item(row,col)->setText(QString::number(getColumnConst(col_index)[row_index], 'g', MAX_DOUBLE_PREC));
                tableWidget->item(row,col)->setBackground(QBrush(VALID_COLOR));
                tableWidget->item(row,col)->setForeground(QBrush(Qt::black));
            }
        }
    }

    disableChecking = false;
}

const std::vector<double> &DataTable::getSelectedColumn() const
{
    return getColumnConst(selectedCol);
}

void DataTable::sortColumnSwapRows(int column, bool ascending)
{
    selectedCol = tableWidget->horizontalHeader()->logicalIndex(column);

    std::vector<uint> index_swap_map(tableWidget->rowCount());
    std::iota(index_swap_map.begin(), index_swap_map.end(), 0);

    if(ascending)
    {
        struct CompAscend
        {
          CompAscend(const DataTable& a): a(a){}
          bool operator()(uint row_a, uint row_b) const
          {
              const std::vector<double> &selected_col = a.getSelectedColumn();
              if(std::isnan(selected_col[row_b]) || std::isnan(selected_col[row_a]))
                  return -1;
              else return selected_col[row_a] < selected_col[row_b];
          }

         const DataTable& a;
        };

        std::sort(index_swap_map.begin(), index_swap_map.end(), CompAscend(*this));
    }
    else
    {
        struct CompDescend
        {
          CompDescend(const DataTable& a): a(a){}
          bool operator()(uint row_a, uint row_b) const
          {
              const std::vector<double> &selected_col = a.getSelectedColumn();
              if(std::isnan(selected_col[row_b]) || std::isnan(selected_col[row_a]))
                  return 1;
              else return selected_col[row_a] > selected_col[row_b];
          }

         const DataTable& a;
        };

        std::sort(index_swap_map.begin(), index_swap_map.end(), CompDescend(*this));
    }

    for(uint i = 0 ; i < index_swap_map.size() ; i++)
        if(index_swap_map[i] != i)
            swapDataRows(i, index_swap_map[i]);

    updateTableFromData();

    emit valEdited(0,column);
}

void DataTable::swapDataRows(uint row1, uint row2)
{
    /* swaps these two rows in all columns */
    for(auto col = values.begin() ; col != values.end() ; col++)
    {
        std::swap((*col)[row1], (*col)[row2]);
    }
}

void DataTable::fillColumnFromRange(int col, Range range)
{
    double val = range.start;
    int row = 0;
    QTableWidgetItem *item;

    col = tableWidget->horizontalHeader()->logicalIndex(col);

    if(col + 1 == tableWidget->columnCount())
        addColumn();

    int end = trunc((range.end - range.start)/range.step) + 1;

    if(end <= 0)
        return;

    disableChecking = true;

    for(int i = 0 ; i < end ; i++)
    {
        getColumn(col)[row] = val;

        item = tableWidget->item(row, col);
        item->setText(QString::number(val, 'g', MAX_DOUBLE_PREC));
        item->setBackground(QBrush(VALID_COLOR));
        item->setForeground(QBrush(Qt::black));

        if(tableWidget->rowCount() == row+1)
            addRow();

        val += range.step;
        row++;
    }

    disableChecking = false;
    emit valEdited(item->row(), item->column());
}

bool DataTable::fillColumnFromExpr(int col, QString expr)
{
    bool ok = false;
    FastTree *tree = treeCreator.getTreeFromExpr(expr, ok, columnNames);

    if(!ok)
        return false;

    col = tableWidget->horizontalHeader()->logicalIndex(col);

    if(col + 1 == tableWidget->columnCount())
        addColumn();

    int row = 0;

    double val;
    std::vector<double> rowVals(columnNames.size());

    for(row = 0 ; row < tableWidget->rowCount() ; row++)
    {
        for(int column = 0 ; column < tableWidget->columnCount() ; column++) { rowVals[column] = getColumnConst(column)[row];}

        calculator.setAdditionnalVarsValues(rowVals);
        val = calculator.calculateFromTree(tree, getColumnConst(col)[row]);
        getColumn(col)[row] = val;
    }

    treeCreator.deleteFastTree(tree);

    emit valEdited(row, col);
    return true;

}

void DataTable::addRow()
{
    insertRow(tableWidget->rowCount());
}

void DataTable::addColumn()
{
    insertColumn(tableWidget->columnCount());
}

void DataTable::checkCell(QTableWidgetItem *item)
{
    if(disableChecking)
        return;

    if(tableWidget->horizontalHeader()->visualIndex(item->column()) + 1 == tableWidget->columnCount())
        addColumn();
    if(item->row()+1 == tableWidget->rowCount())
        addRow();

    QString expr = item->text();
    if(expr.isEmpty())
    {
        item->setBackground(QBrush(backgroundColor));
        item->setForeground(QBrush(textColor));
        getColumn(item->column())[item->row()] = nan("");
    }
    else
    {
        bool ok = false;
        double val = calculator.calculateExpression(expr, ok);

        if(ok)
        {
            item->setBackground(QBrush(VALID_COLOR));
            item->setForeground(QBrush(Qt::black));

            getColumn(item->column())[item->row()] = val;
            disableChecking = true;
            item->setText(QString::number(val, 'g', MAX_DOUBLE_PREC));
            disableChecking = false;
        }
        else
        {
            item->setBackground(QBrush(INVALID_COLOR));
            item->setForeground(QBrush(Qt::black));

            getColumn(item->column())[item->row()] = nan("");
        }
    }

    emit valEdited(item->row(), tableWidget->horizontalHeader()->visualIndex(item->column()));
}

void DataTable::insertRow(int index)
{
    tableWidget->insertRow(index);
    tableWidget->setRowHeight(index, cellHeight);

    disableChecking = true;

    for(uint col = 0 ; col < values.size() ; col++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(" ");
        tableWidget->setItem(index, col, item);

        auto &column = getColumn(col);
        column.insert(column.begin()+index, nan(""));
    }

    disableChecking = false;

    emit newRowCount(tableWidget->rowCount());

    double count = tableWidget->rowCount();

    if(floor(log(count)/log(10)) != floor(log(count-1)/log(10)))
    {
        tableWidget->verticalHeader()->hide();
        tableWidget->verticalHeader()->show();
        checkVerticalHeaderNewWidth();
    }

    checkVerticalHeaderNewWidth();
}

void DataTable::insertColumn(int index)
{
    tableWidget->insertColumn(index);
    columnNames.insert(index, tr("Rename me!"));
    tableWidget->setHorizontalHeaderLabels(columnNames);

    tableWidget->setColumnWidth(index, cellWidth);
    int rowCount = values.front().size();

    values.emplace(column_it(index), std::vector<double>(rowCount, nan("")));

    disableChecking = true;

    for(int row = 0 ; row < rowCount ; row++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(" ");
        tableWidget->setItem(row, index, item);
    }

    disableChecking = false;

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);

    emit newColumnCount(tableWidget->columnCount());
}

void DataTable::removeRow(int index)
{
    tableWidget->removeRow(index);

    for(std::vector<double> &column: values) { column.erase(column.begin() + index); }

    emit newRowCount(tableWidget->rowCount());

    double count = tableWidget->rowCount();

    if(floor(log(count)/log(10)) != floor(log(count-1)/log(10)))
        //this is a correction to a bug in Qt: the vertical header's width doesn't update instantaneously when it visually gets resized
    {
        tableWidget->verticalHeader()->hide();
        tableWidget->verticalHeader()->show();
        checkVerticalHeaderNewWidth();
    }
}

void DataTable::removeColumn(int index)
{
    tableWidget->removeColumn(index);
    columnNames.removeAt(index);

    values.erase(column_it(index));

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);

    emit newColumnCount(tableWidget->columnCount());
}

void DataTable::renameColumn(int index)
{
    bool ok = true;
    QString name = QInputDialog::getText(this, tr("New name for column"), tr("Please enter a name for this column:"), QLineEdit::Normal, "", &ok);

    if(!ok)
        return;
    if(!nameValidator.match(name).hasMatch())
    {
        QMessageBox::information(this, tr("Error"), tr("Column names can only have letters and \"_\""));
        return;
    }

    columnNames[index] = name;
    tableWidget->horizontalHeaderItem(index)->setText(name);

    emit newColumnName(index);

}

void DataTable::resizeColumns(int columnWidth)
{
    cellWidth = columnWidth;

    for(int i = 0 ; i < tableWidget->columnCount(); i++)
    {
        tableWidget->setColumnWidth(i, columnWidth);
    }

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);
}

void DataTable::resizeRows(int rowHeight)
{
    cellHeight = rowHeight;

    for(int i = 0 ; i < tableWidget->rowCount(); i++)
    {
        tableWidget->setRowHeight(i, rowHeight);
    }
}

int DataTable::colVisualIndex(int logicalIndex)
{
    return tableWidget->horizontalHeader()->visualIndex(logicalIndex);
}

int DataTable::colLogicalIndex(int visualIndex)
{
    return tableWidget->horizontalHeader()->logicalIndex(visualIndex);
}

QSize DataTable::getVerticalHeaderSize()
{
    return tableWidget->verticalHeader()->size();
}

QSize DataTable::getHorizontalHeaderSize()
{
    return tableWidget->horizontalHeader()->size();
}

int DataTable::getColumnCount()
{
    return tableWidget->columnCount();
}

int DataTable::getRowCount()
{
    return tableWidget->rowCount();
}
