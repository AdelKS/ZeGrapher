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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include "MathObjects/data.h"

#include <QAbstractTableModel>

#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/qmlsingleton.h"
#include <string>
#include <vector>

namespace zg {

class DataTableModel: public QAbstractTableModel
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(int columns READ columnCount NOTIFY columnCountChanged)

public:
  /// @brief QML uses the global instance instead of creating one
  static DataTableModel* create(QQmlEngine*, QJSEngine*);

  explicit DataTableModel(QObject *parent);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

  /// @brief empties the cells, one edit per column
  Q_INVOKABLE void clearCells(const QModelIndexList& cells);

  /// @brief deletes the selected columns, rows and cells
  /// @param cells  every selected cell, the ones in 'rows' and 'columns' included
  Q_INVOKABLE void deleteSelection(const QModelIndexList& cells, const QModelIndexList& rows,
                                   const QModelIndexList& columns);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QHash<int, QByteArray> roleNames() const override;

  void registerTableColumn(zg::mathobj::Data*);
  void deregisterTableColumn(zg::mathobj::Data*);

signals:
  void columnCountChanged();

protected:
  static QString headerName(const zg::mathobj::Data*);

  /// @brief re-reads the header of an already registered column
  void refreshColumnName(const zg::mathobj::Data*);

  std::vector<QString> columnNames;
  std::vector<zg::mathobj::Data*> tableColumns;

};

/// @brief set by Globals
inline DataTableModel* dataTableModel = nullptr;

inline DataTableModel* DataTableModel::create(QQmlEngine*, QJSEngine*)
{
  return qmlSingleton(dataTableModel);
}

} // namespace zg
