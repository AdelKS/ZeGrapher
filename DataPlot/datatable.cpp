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

int refCol;

DataTable::DataTable(Information *info, int rowCount, int columnCount, int rowHeight, int columnWidth)
{
    information = info;
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    setMinimumSize(0,0);
    calculator = new ExprCalculator(false, information->getFuncsList());
    treeCreator = new TreeCreator(ObjectType::DATA_TABLE_EXPR);

    tableWidget = new QTableWidget(rowCount,columnCount);

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
        values << QList<double>();
        for(int row = 0 ; row < rowCount ; row++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(" ");
            tableWidget->setItem(row, col, item);
            values[col] << nan("") ;
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

QList<QStringList> DataTable::getData()
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

            if(nameValidator.exactMatch(data[0][column]))
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
        qSort(values[col].begin(), values[col].end(), doubleCompareAscend);
    else qSort(values[col].begin(), values[col].end(), doubleCompareDescend);

    disableChecking = true;

    for(int row = 0 ; row < tableWidget->rowCount(); row++)
    {
        if(std::isnan(values[col][row]))
        {
            tableWidget->item(row,col)->setText("");
            tableWidget->item(row,col)->setBackgroundColor(backgroundColor);
            tableWidget->item(row,col)->setTextColor(textColor);
        }
        else
        {
            tableWidget->item(row,col)->setText(QString::number(values[col][row], 'g', MAX_DOUBLE_PREC));
            tableWidget->item(row,col)->setBackgroundColor(VALID_COLOR);
            tableWidget->item(row,col)->setTextColor(Qt::black);
        }
    }

    disableChecking = false;

    emit valEdited(0, col);
}

int listCompareAscend(QList<double> a, QList<double>b)
{
    if(std::isnan(b[refCol]))
        return -1;
    else return a[refCol] < b[refCol];
}

int listCompareDescend(QList<double> a, QList<double>b)
{
    if(std::isnan(b[refCol]))
        return 1;
    else return a[refCol] > b[refCol];
}

void DataTable::sortColumnSwapRows(int column, bool ascending)
{
    column = tableWidget->horizontalHeader()->logicalIndex(column);

    QList<QList<double> > vals; //this time it will be vals[row][column] just to apply qSort on it, since QSorting values can be made only on columns...

    for(int row = 0 ; row < tableWidget->rowCount(); row++)
    {
        vals << QList<double>();
        for(int col = 0 ; col < tableWidget->columnCount(); col++)
            vals[row] << values[col][row];
    }

    refCol = column;
    if(ascending)
        qSort(vals.begin(), vals.end(), listCompareAscend);
    else qSort(vals.begin(), vals.end(), listCompareDescend);

    disableChecking = true;

    for(int row = 0 ; row < tableWidget->rowCount(); row++)
    {
        for(int col = 0 ; col < tableWidget->columnCount(); col++)
        {
            values[col][row] = vals[row][col];
            if(std::isnan(values[col][row]))
            {
                tableWidget->item(row,col)->setText("");
                tableWidget->item(row,col)->setBackgroundColor(backgroundColor);
                tableWidget->item(row,col)->setTextColor(textColor);
            }
            else
            {
                tableWidget->item(row,col)->setText(QString::number(values[col][row], 'g', MAX_DOUBLE_PREC));
                tableWidget->item(row,col)->setBackgroundColor(VALID_COLOR);
                tableWidget->item(row,col)->setTextColor(Qt::black);
            }
        }
    }

    disableChecking = false;

    emit valEdited(0,column);
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
        values[col][row] = val;

        item = tableWidget->item(row, col);
        item->setText(QString::number(val, 'g', MAX_DOUBLE_PREC));
        item->setBackgroundColor(VALID_COLOR);
        item->setTextColor(Qt::black);

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
    FastTree *tree = treeCreator->getTreeFromExpr(expr, ok, columnNames);

    if(!ok)
        return false;

    col = tableWidget->horizontalHeader()->logicalIndex(col);

    if(col + 1 == tableWidget->columnCount())
        addColumn();

    int row = 0;

    disableChecking = true;

    double val;
    QList<double> rowVals;

    for(row = 0 ; row < tableWidget->rowCount() ; row++)
    {
        rowVals.clear();
        for(int column = 0 ; column < tableWidget->columnCount() ; column++) { rowVals << values[column][row];}

        calculator->setAdditionnalVarsValues(rowVals);
        val = calculator->calculateFromTree(tree, values[col][row]);
        values[col][row] = val;
        QTableWidgetItem *item = tableWidget->item(row, col);

        if(std::isnan(val))
        {
            item->setText("");
            item->setBackgroundColor(backgroundColor);
            item->setTextColor(textColor);
        }
        else
        {
            item->setText(QString::number(val, 'g', MAX_DOUBLE_PREC));
            item->setBackgroundColor(VALID_COLOR);
            item->setTextColor(Qt::black);
        }

    }

    disableChecking = false;
    treeCreator->deleteFastTree(tree);

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

QList<QList<double> > &DataTable::getValues()
{
    return values;
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
        item->setBackgroundColor(backgroundColor);
        item->setTextColor(textColor);
        values[item->column()][item->row()] = nan("");
    }
    else
    {
        bool ok = false;
        double val = calculator->calculateExpression(expr, ok);

        if(ok)
        {
            item->setBackgroundColor(VALID_COLOR);
            item->setTextColor(Qt::black);

            values[item->column()][item->row()] = val;
            disableChecking = true;
            item->setText(QString::number(val, 'g', MAX_DOUBLE_PREC));
            disableChecking = false;
        }
        else
        {
            item->setBackgroundColor(INVALID_COLOR);
            item->setTextColor(Qt::black);

            values[item->column()][item->row()] = nan("");
        }
    }

    emit valEdited(item->row(), tableWidget->horizontalHeader()->visualIndex(item->column()));
}

void DataTable::insertRow(int index)
{
    tableWidget->insertRow(index);
    tableWidget->setRowHeight(index, cellHeight);

    disableChecking = true;

    for(int col = 0 ; col < values.size() ; col++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(" ");
        tableWidget->setItem(index, col, item);
        values[col].insert(index, nan(""));
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
    int rowCount = values[0].size();
    values.insert(index, QList<double>());

    disableChecking = true;

    for(int row = 0 ; row < rowCount ; row++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(" ");
        tableWidget->setItem(row, index, item);
        values[index] << nan("") ;
    }

    disableChecking = false;

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);

    emit newColumnCount(tableWidget->columnCount());
}

void DataTable::removeRow(int index)
{
    tableWidget->removeRow(index);

    for(int col = 0 ; col < values.size() ; col++) { values[col].removeAt(index); }

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

    values.removeAt(index);

    tableWidget->setFixedWidth(tableWidget->columnCount() * cellWidth + tableWidget->verticalHeader()->width() + 10);

    emit newColumnCount(tableWidget->columnCount());
}

void DataTable::renameColumn(int index)
{
    bool ok = true;
    QString name = QInputDialog::getText(this, tr("New name for column"), tr("Please enter a name for this column:"), QLineEdit::Normal, "", &ok);

    if(!ok)
        return;
    if(!nameValidator.exactMatch(name))
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

DataTable::~DataTable()
{
    delete calculator;
    delete treeCreator;
}
