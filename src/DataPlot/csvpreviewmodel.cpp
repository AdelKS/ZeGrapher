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
#include "information.h"

#include <QFile>
#include <QDir>
#include <QCoreApplication>


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

QVariant CsvPreviewModel::data(const QModelIndex& index, int role) const
{
  if (not index.isValid() or role != Qt::DisplayRole)
    return {};

  return csvData.at(index.row()).at(index.column());
}

QVariant CsvPreviewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return {};

  if (orientation == Qt::Orientation::Horizontal)
    return columnNames.at(section);
  else
    return section;
}

Qt::ItemFlags CsvPreviewModel::flags(const QModelIndex& index) const
{
  if (not index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QHash<int, QByteArray> CsvPreviewModel::roleNames() const
{
  return { {Qt::DisplayRole, "display"} };
}

void CsvPreviewModel::setMaxRowCount(int count)
{
  maxRowCount = count;
  readCsvFile();
}

void CsvPreviewModel::setRowSkipCount(int count)
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

  // every option setter rereads the file, so only this one reports failures,
  // once per file
  if (const QString error = readCsvFile(); not error.isEmpty())
    information->readFailed(csvFile.toLocalFile(), error);
}

QString CsvPreviewModel::readCsvFile()
{
  QFile file(csvFile.toLocalFile());
  QString error;

  if(not csvFile.isEmpty() and file.open(QFile::ReadOnly | QFile::Text))
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
  // an empty path means no file is picked yet, which isn't an error
  else if (not csvFile.isEmpty())
    error = file.errorString();

  splitCsvFile();

  return error;
}

void CsvPreviewModel::setSeparator(QString sep)
{
  // support tab separator by looking for the user writing "\t"
  separator = sep.replace("\\t", "\t");
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
    columnNames = splitCsvLine(*it, separator);
    column_count = columnNames.size();
    it++;
    row++;
  }

  for (; it != csvRawData.cend() and row < maxRowCount + rowSkipCount + csvHasHeaderRow ; it++, row++)
  {
    csvData << splitCsvLine(*it, separator);
    column_count = std::max(csvData.back().size(), column_count);
  }

  for (auto& rowData: csvData)
    rowData.resize(column_count);

  columnNames.resize(column_count);

  endResetModel();
}

void CsvPreviewModel::loadIntoWorld()
{
  if (loadingState != FREE)
    return;

  QFile file(csvFile.toLocalFile());

  if(file.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream in(&file);

    // Get the total size of the file
    qint64 totalBytes = file.size();
    qint64 processedBytes = 0; // Tracks processed bytes

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
    progressPercentage = 0;

    loadingState = READING_CSV_FILE;
    emit loadingStateChanged();
    QCoreApplication::processEvents();

    auto start = std::chrono::steady_clock::now();

    while (not in.atEnd())
    {
      QString line = in.readLine();

      processedBytes += line.size() + 1; // Account for the newline character

      if (totalBytes > 0)
      {
        // Cap at 50% since we still need to add to the world
        progressPercentage = static_cast<int>((processedBytes * 50) / totalBytes);
        progressPercentage = qMin(progressPercentage, 50);
      }

      auto now = std::chrono::steady_clock::now();

      using namespace std::chrono_literals;
      if (now - start > 10ms)
      {
        emit progressPercentageChanged();
        start = now;
        QCoreApplication::processEvents();
      }

      if (line_num < rowSkipCount)
        ;
      else if (line_num == rowSkipCount and csvHasHeaderRow)
        names = splitCsvLine(line, separator);
      else
        append_values(splitCsvLine(line, separator));

      line_num++;
    }

    // a header wider than every row, or a file with a header and no row
    if (data.size() < size_t(names.size()))
      data.resize(names.size(), std::vector<std::string>(data.empty() ? 0 : data.front().size()));

    loadingState = ADDING_TO_WORLD;
    emit loadingStateChanged();

    if (not dataSheet)
    {
      dataSheet = mathWorld->addMathObject(MathObject::DATASHEET)->getDataSheet();
      dataSheet->setNotes(tr("imported from %1").arg(displayPath(csvFile)));
    }

    // processEvents() below lets the user delete the sheet
    for (size_t i = 0 ; i != data.size() and dataSheet ; i++)
    {
      mathobj::Data* column = dataSheet->addColumn();
      column->style.setVisible(false);
      column->setName(names[i]);
      column->setData(std::move(data[i]));
      progressPercentage = 50 + i*50/data.size();
      emit progressPercentageChanged();
      QCoreApplication::processEvents();
    }

    file.close();

    loadingState = FREE;
    dataSheet = nullptr;

    emit loadingStateChanged();
  }
  else information->readFailed(csvFile.toLocalFile(), file.errorString());
}

QString CsvPreviewModel::displayPath(const QUrl& url) const
{
  const QString path = url.toLocalFile();

#ifndef Q_OS_WIN
  if (const QString rel = QDir::home().relativeFilePath(path); not rel.startsWith("../"))
    return "~/" + rel;
#endif

  return QDir::toNativeSeparators(path);
}

QStringList CsvPreviewModel::splitCsvLine(QStringView line, QStringView sep)
{
  if (sep.isEmpty())
    return {line.toString()};

  // reads the quoted part of a field, leaving 'i' right after the closing quote
  auto read_quoted = [&](qsizetype& i)
  {
    QString content;
    for (i++ ; i < line.size() ; i++)
    {
      if (line[i] != '"')
        content += line[i];
      else if (i + 1 < line.size() and line[i+1] == '"')
        content += line[i++];
      else { i++; break; }
    }
    return content;
  };

  QStringList fields;

  for (qsizetype i = 0 ;;)
  {
    QString field = line.sliced(i).startsWith('"') ? read_quoted(i) : QString();

    const qsizetype sep_pos = line.indexOf(sep, i);
    field += line.sliced(i, (sep_pos < 0 ? line.size() : sep_pos) - i);

    fields << std::move(field);

    if (sep_pos < 0)
      return fields;

    i = sep_pos + sep.size();
  }
}



}
