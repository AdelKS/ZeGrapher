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
#include <QPointer>
#include <QUrl>

#include <QtQmlIntegration/qqmlintegration.h>

#include "MathObjects/datasheet.h"
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
  Q_PROPERTY(zg::mathobj::DataSheet* dataSheet READ getDataSheet WRITE setDataSheet)

public:
  CsvPreviewModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QHash<int, QByteArray> roleNames() const override;

  /// @brief split a single CSV line, honoring RFC 4180 quoting
  /// @note a field wrapped in double quotes may contain separators,
  //        and a doubled quote within it stands for a literal one.
  //        Quotes spanning several lines are not supported: one line is always one row.
  static QStringList splitCsvLine(QStringView line, QStringView sep);

  /// @brief a file's path as the user writes it: native separators, and '~' for
  ///        the home folder outside Windows
  Q_INVOKABLE QString displayPath(const QUrl& file) const;

  enum LoadingState {FREE, READING_CSV_FILE, ADDING_TO_WORLD};

  Q_ENUM(LoadingState);

  LoadingState getLoadingState() const { return loadingState; }
  int getProgressPercentage() const { return progressPercentage; }

  mathobj::DataSheet* getDataSheet() const { return dataSheet; }
  void setDataSheet(mathobj::DataSheet* sheet) { dataSheet = sheet; }

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

  /// @returns why the file could not be opened, empty when it was read or when
  ///          no file is set
  QString readCsvFile();

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
  QList<QStringList> csvData;

  /// @brief the sheet to load the columns into, a new one when empty
  /// @note the user can delete the sheet while the import settings are open
  QPointer<mathobj::DataSheet> dataSheet;

};

} // namespace zg
