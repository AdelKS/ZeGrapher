/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

#include "datatablemodel.h"

namespace zg {

DataTableModel::DataTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

int DataTableModel::rowCount(const QModelIndex& parent) const
{
  // no nested cells
  if (parent.isValid())
    return 0;

  size_t rows = 0;
  for (const zg::mathobj::Data* data: tableColumns)
    rows = std::max(rows, data->getData().size());

  return int(rows);
}

int DataTableModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return tableColumns.size();
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const
{
  if (not index.isValid() or (role != Qt::DisplayRole and role != Qt::EditRole))
    return {};

  const std::vector<std::string>& values = tableColumns.at(index.column())->getData();
  return size_t(index.row()) < values.size() ? QString::fromStdString(values[index.row()]) : QString();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
  Q_ASSERT(index.column() < columnCount());

  const std::string str = value.toString().toStdString();

  tableColumns.at(index.column())->setValue(index.row(), str);

  emit dataChanged(index, index);

  if (not str.empty())
  {
    int row_num = rowCount();
    if (index.row()+1 == row_num)
      insertRow(row_num);
  }

  return true;
}

bool DataTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if (parent.isValid())
    return false;

  int old_row_count = rowCount();

  beginInsertRows(parent, row, row + count - 1);

  // a column that ends above 'row' keeps its length, unless it is the longest
  for (mathobj::Data* dataObj: tableColumns)
    if (const size_t size = dataObj->getData().size(); size_t(row) < size or int(size) == old_row_count)
      dataObj->insertValues(row, count);

  endInsertRows();

  Q_ASSERT(rowCount() == old_row_count + count);

  // QML TableView binds delegates to a cell position, not to a persistent
  // model index, so endInsertRows() only fixes the row count/geometry — the
  // rows shifted down keep showing their old cached values. Refresh the
  // inserted rows plus everything below them so their content re-reads.
  if (old_row_count > 0)
  {
    QModelIndex topLeft = index(row, 0);
    QModelIndex bottomRight = index(old_row_count - 1 + count, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
  }

  return true;
}

void DataTableModel::removeRow(int row)
{
  if (row < 0 or row >= rowCount())
    return;

  beginRemoveRows(QModelIndex(), row, row);

  for (mathobj::Data* dataObj: tableColumns)
    dataObj->removeValue(row);

  endRemoveRows();

  // QML TableView binds delegates to a cell position, not to a persistent
  // model index, so endRemoveRows() only fixes the row count/geometry — the
  // rows shifted up keep showing their old cached values. Refresh everything
  // from the removal point down to the new last row so their content re-reads.
  if (row < rowCount())
  {
    QModelIndex topLeft = index(row, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
  }
}

void DataTableModel::clearCells(QModelIndexList list)
{
  for (auto index: list)
    setData(index, QString());
}

void DataTableModel::removeCell(const QModelIndex& cell)
{
  if (size_t(cell.column()) >= tableColumns.size())
    return;

  mathobj::Data* column = tableColumns[cell.column()];
  if (size_t(cell.row()) >= column->getData().size())
    return;

  // rowCount() is the size of the longest column, so it drops when no other
  // column is as long as this one
  size_t remaining = 0;
  for (const zg::mathobj::Data* data: tableColumns)
    remaining = std::max(remaining, data->getData().size() - (data == column));

  const int rows = rowCount();
  const bool shrinks = int(remaining) < rows;

  if (shrinks)
    beginRemoveRows(QModelIndex(), rows - 1, rows - 1);

  column->removeValue(cell.row());

  if (shrinks)
    endRemoveRows();

  if (cell.row() < rowCount())
    emit dataChanged(cell, index(rowCount() - 1, cell.column()));
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return {};

  if (orientation == Qt::Orientation::Horizontal)
    return columnNames.at(section);
  else
    return section;
}

void DataTableModel::registerTableColumn(zg::mathobj::Data* d)
{
  if (std::ranges::find(tableColumns, d) != tableColumns.end()) return;

  beginInsertColumns(QModelIndex(), tableColumns.size(), tableColumns.size());
  tableColumns.push_back(d);
  columnNames.push_back(headerName(d));
  endInsertColumns();

  // a column is named by the Data object, we just follow it
  connect(d, &mathobj::Data::nameChanged, this, [this, d] { refreshColumnName(d); });
  connect(d, &mathobj::Data::stateChanged, this, [this, d] { refreshColumnName(d); });

  // a column length change affects the rows of every column, which a single
  // reset covers
  connect(d, &mathobj::Data::dataAboutToBeReplaced, this, &DataTableModel::beginResetModel);
  connect(d, &mathobj::Data::dataReplaced, this, &DataTableModel::endResetModel);

  emit columnCountChanged();
}

void DataTableModel::deregisterTableColumn(zg::mathobj::Data* d)
{
  if (auto it = std::ranges::find(tableColumns, d); it != tableColumns.end())
  {
    disconnect(d, nullptr, this, nullptr);

    int index = size_t(it - tableColumns.begin());
    beginRemoveColumns(QModelIndex(), index, index);
    tableColumns.erase(it);
    columnNames.erase(columnNames.begin() + index);
    endRemoveColumns();
    emit columnCountChanged();
    if (index < columnCount())
      emit headerDataChanged(Qt::Orientation::Horizontal, index, columnCount()-1);
  }
}

QString DataTableModel::headerName(const zg::mathobj::Data* d)
{
  // a column whose name doesn't hold up has no header to show
  return d->getState().isValid() ? d->getName() : QString();
}

void DataTableModel::refreshColumnName(const zg::mathobj::Data* d)
{
  auto it = std::ranges::find(tableColumns, d);
  if (it == tableColumns.end())
    return;

  const int index = it - tableColumns.begin();
  const QString name = headerName(d);
  if (columnNames.at(index) == name)
    return;

  columnNames[index] = name;
  emit headerDataChanged(Qt::Orientation::Horizontal, index, index);
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex& index) const
{
  if (not index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

QHash<int, QByteArray> DataTableModel::roleNames() const
{
  return { {Qt::DisplayRole, "display"}, {Qt::EditRole, "display"} };
}

void DataTableModel::requestUiInitiatedDelete(size_t column)
{
  if (column < tableColumns.size())
    tableColumns[column]->requestUiInitiatedDelete();
}

}
