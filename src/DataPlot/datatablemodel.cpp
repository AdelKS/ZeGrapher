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

  size_t data_size = 0;
  for (const zg::mathobj::Data* data: tableColumns)
    if (std::optional<size_t> opt_size = data->zcMathObj.get_data_size())
      if (*opt_size > data_size)
        data_size = *opt_size;

  return int(data_size);
}

int DataTableModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return tableColumns.size();
}

QVariant DataTableModel::data(const QModelIndex& index, int) const
{
  Q_ASSERT(index.column() < columnCount());

  const mathobj::Data* dataObj = tableColumns.at(index.column());
  std::optional<std::string> opt_str = dataObj->zcMathObj.get_data_point(index.row());
  if (opt_str)
    return QString::fromStdString(*opt_str);
  else return QString();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
  Q_ASSERT(index.column() < columnCount());

  const mathobj::Data* dataObj = tableColumns.at(index.column());
  dataObj->zcMathObj.set_data_point(index.row(), value.toString().toStdString());

  emit dataChanged(index, index);

  return true;
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int) const
{
  if (orientation == Qt::Orientation::Horizontal)
    return columnNames.at(section);
  else
    return section;
}

void DataTableModel::registerTableColumn(zg::mathobj::Data* d)
{
  beginInsertColumns(QModelIndex(), tableColumns.size(), tableColumns.size());
  tableColumns.push_back(d);
  columnNames.push_back(d->getName());
  endInsertColumns();
  emit columnCountChanged();
}

void DataTableModel::deregisterTableColumn(zg::mathobj::Data* d)
{
  if (auto it = std::ranges::find(tableColumns, d); it != tableColumns.end())
  {
    int index = size_t(it - tableColumns.begin());
    beginRemoveColumns(QModelIndex(), index, index);
    tableColumns.erase(it);
    columnNames.erase(columnNames.begin() + index);
    endRemoveColumns();
    emit columnCountChanged();
  }
}

void DataTableModel::setColumnName(zg::mathobj::Data* d, QString name)
{
  if (auto it = std::ranges::find(tableColumns, d); it != tableColumns.end())
  {
    int index = size_t(it - tableColumns.begin());
    if (columnNames.at(index) != name)
    {
      columnNames[index] = name;
      emit headerDataChanged(Qt::Orientation::Horizontal, index, index);
    }
  }
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &) const
{
  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

QHash<int, QByteArray> DataTableModel::roleNames() const
{
  return { {Qt::DisplayRole, "display"}, {Qt::EditRole, "display"} };
}

}
