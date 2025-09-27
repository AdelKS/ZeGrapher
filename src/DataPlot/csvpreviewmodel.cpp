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

#include "csvpreviewmodel.h"
#include "MathObjects/mathobject.h"
#include "MathObjects/mathworld.h"

#include <QFile>

namespace zg {

CsvPreviewModel::CsvPreviewModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

int CsvPreviewModel::rowCount(const QModelIndex& parent) const
{
  // no nested cells
  if (parent.isValid())
    return 0;

  return csvData.size();
}

int CsvPreviewModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;

  return csvData.isEmpty() ? 0 : csvData.front().size();
}

QVariant CsvPreviewModel::data(const QModelIndex& index, int) const
{
  Q_ASSERT(index.column() < columnCount());
  Q_ASSERT(index.row() < rowCount());

  return QString(csvData.at(index.row()).at(index.column()));
}

QVariant CsvPreviewModel::headerData(int section, Qt::Orientation orientation, int) const
{
  if (orientation == Qt::Orientation::Horizontal)
    return columnNames.at(section);
  else
    return section;
}

Qt::ItemFlags CsvPreviewModel::flags(const QModelIndex &) const
{
  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QHash<int, QByteArray> CsvPreviewModel::roleNames() const
{
  return { {Qt::DisplayRole, "display"} };
}

void CsvPreviewModel::setMaxRowCount(size_t count)
{
  maxRowCount = count;
  readCsvFile();
}

void CsvPreviewModel::setRowSkipCount(size_t count)
{
  rowSkipCount = count;
  readCsvFile();
}

void CsvPreviewModel::setCsvHasHeaderRow(bool b)
{
  csvHasHeaderRow = b;
  readCsvFile();
}

void CsvPreviewModel::setCsvFile(QUrl file)
{
  csvRawData.clear();

  csvFile = file;

  readCsvFile();
}

void CsvPreviewModel::readCsvFile()
{
  QFile file(csvFile.toLocalFile());

  if(file.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream in(&file);
    int line_num = 0;
    while (not in.atEnd() && line_num < rowSkipCount + maxRowCount + csvHasHeaderRow)
    {
      QString line = in.readLine();
      if (line_num == csvRawData.size())
        csvRawData << line;

      line_num++;
    }

    file.close();
  }

  splitCsvFile();
}

void CsvPreviewModel::setSeparator(QString sep)
{
  separator = sep;
  splitCsvFile();
}

void CsvPreviewModel::splitCsvFile()
{
  beginResetModel();

  csvData.clear();
  columnNames.clear();

  qsizetype column_count = 0;

  auto it = csvRawData.cbegin();
  int row = 0;

  if (rowSkipCount and it != csvRawData.cend())
    while (++it != csvRawData.cend() and ++row < rowSkipCount){}

  if (it == csvRawData.cend())
  {
    endResetModel();
    return;
  }

  if (csvHasHeaderRow)
  {
    columnNames = it->split(separator);
    column_count = columnNames.size();
    it++;
    row++;
  }

  for (; it != csvRawData.cend() and row < maxRowCount + rowSkipCount + csvHasHeaderRow ; it++, row++)
  {
    csvData << QStringView(*it).split(separator);
    column_count = std::max(csvData.back().size(), column_count);
  }

  for (auto& rowData: csvData)
    rowData.resize(column_count);

  columnNames.resize(column_count);

  endResetModel();
}

void CsvPreviewModel::loadIntoWorld() const
{
  QFile file(csvFile.toLocalFile());

  if(file.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream in(&file);

    std::vector<std::vector<std::string>> data;
    QStringList names;

    auto append_values = [&](QStringList values)
    {
      // make sure that size of values is the same as data size
      // top up with empty values if needed

      if (data.size() < size_t(values.size()))
        data.resize(size_t(values.size()), std::vector<std::string>(!data.empty() ? data.front().size() : 0));

      else if (size_t(values.size()) < data.size())
        values.resize(data.size());

      if (names.size() < values.size())
        names.resize(values.size());

      assert(data.size() == size_t(values.size()));

      for (int i = 0 ; i != values.size(); i++)
        data[i].push_back(values[i].toStdString());
    };

    int line_num = 0;

    while (not in.atEnd())
    {
      QString line = in.readLine();

      if (line_num < rowSkipCount)
        ;
      else if (line_num == rowSkipCount and csvHasHeaderRow)
        names = line.split(separator);
      else
        append_values(line.split(separator));

      line_num++;
    }

    assert(data.size() == size_t(names.size()));

    for (size_t i = 0 ; i != data.size() ; i++)
    {
      MathObject* obj = mathWorld.addMathObject(MathObject::DATA);
      obj->getData()->setData(names[i], std::move(data[i]));
      qApp->processEvents();
    }

    file.close();
  }

}

}
