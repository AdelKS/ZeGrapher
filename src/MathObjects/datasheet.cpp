/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "MathObjects/datasheet.h"
#include "Utils/yaml.h"

#include <QSaveFile>

namespace zg {
namespace mathobj {

DataSheet::DataSheet(QObject *parent)
  : QAbstractListModel(parent)
{}

int DataSheet::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return 0;

  return columns.size();
}

QVariant DataSheet::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  return QVariant::fromValue(columns.at(index.row()));
}

QHash<int, QByteArray> DataSheet::roleNames() const
{
  return { {Qt::DisplayRole, "dataPtr"} };
}

int DataSheet::indexOfColumn(const Data* d) const
{
  auto it = std::ranges::find(columns, d);
  if (it == columns.end())
    return -1;

  return int(it - columns.begin());
}

Data* DataSheet::addColumn()
{
  Data* d = new Data(this);

  beginInsertRows(QModelIndex(), columns.size(), columns.size());
  columns.push_back(d);
  endInsertRows();

  // a column's edits are the sheet's edits, MathWorld syncs on them
  connect(d, &Data::updated, this, &DataSheet::updated);
  connect(d, &QObject::destroyed, this, [this, d] { forgetColumn(d); });

  emit columnsChanged();
  emit updated();

  return d;
}

void DataSheet::deleteColumn(Data* d)
{
  if (indexOfColumn(d) == -1)
    return;

  disconnect(d, nullptr, this, nullptr);
  forgetColumn(d);

  // delay the delete so QML can stop using the column first
  d->deleteLater();
}

void DataSheet::forgetColumn(const Data* d)
{
  const int index = indexOfColumn(d);
  if (index == -1)
    return;

  beginRemoveRows(QModelIndex(), index, index);
  columns.erase(columns.begin() + index);
  endRemoveRows();

  emit columnsChanged();
  emit updated();
}

void DataSheet::moveColumn(int from, int to)
{
  const int n = int(columns.size());
  if (from == to or from < 0 or from >= n or to < 0 or to >= n)
    return;

  // QAbstractItemModel::beginMoveRows expects the destination row index in
  // pre-move coordinates; when moving down, that's one past the target slot.
  const int qmlTo = from < to ? to + 1 : to;
  if (not beginMoveRows(QModelIndex(), from, from, QModelIndex(), qmlTo))
    return;

  std::swap(columns[from], columns[to]);

  endMoveRows();

  emit updated();
}

void DataSheet::sync()
{
  for (Data* d: columns)
    d->sync();
}

bool DataSheet::isValid() const
{
  return std::ranges::all_of(columns, [](Data* d) { return d->getState().isValid(); });
}

void DataSheet::exportCSV(QUrl fileName) const
{
  QSaveFile file(fileName.toLocalFile());
  if (not file.open(QFile::WriteOnly | QFile::Text))
    return;

  auto write = [&](std::string_view s) { file.write(s.data(), s.size()); };

  // a field that holds a separator or a quote goes between quotes, its own quotes doubled
  auto escape_cell = [](std::string_view field)
  {
    if (field.find_first_of(",\"") == std::string_view::npos)
      return std::string(field);

    std::string escaped = "\"";
    for (char c: field)
    {
      if (c == '"')
        escaped += '"';
      escaped += c;
    }
    return escaped += '"';
  };

  auto write_row = [&](auto&& cell_of)
  {
    bool first = true;
    for (const Data* d: columns)
    {
      if (not first) write(",");
      first = false;
      write(escape_cell(cell_of(d)));
    }
    write("\n");
  };

  write_row([](const Data* d) { return d->getName().toStdString(); });

  size_t rows = 0;
  for (const Data* d: columns)
    rows = std::max(rows, d->getData().size());

  // the columns rarely end together, the shorter ones trail empty cells
  for (size_t row = 0 ; row != rows ; row++)
    write_row([&](const Data* d) -> std::string_view
    {
      const std::vector<std::string>& values = d->getData();
      return row < values.size() ? values[row] : std::string_view();
    });

  file.commit();
}

DataSheet::POD DataSheet::exportPod() const
{
  std::vector<Data::POD> columnPods;
  columnPods.reserve(columns.size());
  for (const Data* d: columns)
    columnPods.push_back(d->exportPod());

  return POD {
    .notes = yml::not_default(notes),
    .columns = yml::not_default(columnPods)
  };
}

void DataSheet::importPod(DataSheet::POD p)
{
  if (p.notes)
    setNotes(QString::fromStdString(*p.notes));

  if (p.columns)
    for (Data::POD& column: *p.columns)
      addColumn()->importPod(std::move(column));
}

void DataSheet::setNotes(QString n)
{
  if (n == notes) return;

  notes = n;

  emit notesChanged();
}

}
}
