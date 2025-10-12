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

#pragma once

#include <QAbstractTableModel>
#include <QUrl>

#include <QtQml/qqmlregistration.h>

namespace zg {

class CsvPreviewModel: public QAbstractTableModel
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(int rowSkipCount WRITE setRowSkipCount MEMBER rowSkipCount)
  Q_PROPERTY(int maxRowCount WRITE setMaxRowCount MEMBER maxRowCount)
  Q_PROPERTY(bool csvHasHeaderRow WRITE setCsvHasHeaderRow MEMBER csvHasHeaderRow)
  Q_PROPERTY(QString separator WRITE setSeparator MEMBER separator)
  Q_PROPERTY(QUrl csvFile WRITE setCsvFile MEMBER csvFile)
  Q_PROPERTY(LoadingState loadingState READ getLoadingState NOTIFY loadingStateChanged)
  Q_PROPERTY(int progressPercentage READ getProgressPercentage NOTIFY progressPercentageChanged)

public:
  CsvPreviewModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QHash<int, QByteArray> roleNames() const override;

  enum LoadingState {FREE, READING_CSV_FILE, ADDING_TO_WORLD};

  Q_ENUM(LoadingState);

  Q_INVOKABLE LoadingState getLoadingState() const { return loadingState; }
  Q_INVOKABLE int getProgressPercentage() const { return progressPercentage; }

public slots:
  void setMaxRowCount(size_t);
  void setRowSkipCount(size_t);
  void setCsvHasHeaderRow(bool);
  void setCsvFile(QUrl);
  void setSeparator(QString);
  void loadIntoWorld();

signals:
  void loadingStateChanged();
  void progressPercentageChanged();

protected:
  void splitCsvFile();
  void readCsvFile();

  LoadingState loadingState = FREE;
  int progressPercentage = 0;

  int maxRowCount = 10;
  int rowSkipCount = 0;
  QString separator = ",";
  bool csvHasHeaderRow = true;
  QUrl csvFile;

  /// @brief raw csv data, one string per line
  QStringList csvRawData;
  QStringList columnNames;

  /// @brief split csv data, rows[columns]
  /// @note the views refer to csvRawData
  QList<QList<QStringView>> csvData;

};

} // namespace zg
