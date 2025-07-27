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
#include "information.h"

namespace zg {

DataTableModel::DataTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{

}

int DataTableModel::rowCount(const QModelIndex&) const
{
  size_t data_size = 0;
  for (const zg::mathobj::Data* data: information.getTableColumns())
    if (std::optional<size_t> opt_size = data->zcMathObj.get_data_size())
      if (*opt_size > data_size)
        data_size = *opt_size;

  return int(data_size);
}

int DataTableModel::columnCount(const QModelIndex&) const
{
  return information.getTableColumns().size();
}

QVariant DataTableModel::data(const QModelIndex& index, int role) const
{
  Q_ASSERT(index.column() < columnCount());

  if (role != Qt::DisplayRole)
    return {};

  const mathobj::Data* dataObj = information.getTableColumns().at(index.column());
  std::optional<std::string> opt_str = dataObj->zcMathObj.get_data_point(index.row());
  if (opt_str)
    return QString::fromStdString(*opt_str);
  else return QString();
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role != Qt::EditRole)
    return false;

  Q_ASSERT(index.column() < columnCount());

  const mathobj::Data* dataObj = information.getTableColumns().at(index.column());
  dataObj->zcMathObj.set_data_point(index.row(), value.toString().toStdString());
  return true;
}

Qt::ItemFlags DataTableModel::flags(const QModelIndex &) const
{
  return Qt::ItemIsEditable;
}

QHash<int, QByteArray> DataTableModel::roleNames() const
{
  return { {Qt::DisplayRole, "displayStr"}, {Qt::EditRole, "displayStr"} };
}

}
