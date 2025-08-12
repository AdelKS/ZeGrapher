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

#include "MathObjects/data.h"

#include <QAbstractTableModel>

#include <QtQml/qqmlregistration.h>
#include <vector>

namespace zg {

class DataTableModel: public QAbstractTableModel
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(int columns READ columnCount NOTIFY columnCountChanged)

public:
  DataTableModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
  Q_INVOKABLE void clearCells(QModelIndexList list);

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void registerTableColumn(zg::mathobj::Data*);
  Q_INVOKABLE void deregisterTableColumn(zg::mathobj::Data*);
  Q_INVOKABLE void setColumnName(zg::mathobj::Data*, QString);

signals:
  void columnCountChanged();

protected:
  std::vector<QString> columnNames;
  std::vector<zg::mathobj::Data*> tableColumns;

};

inline DataTableModel dataTableModel;

/// @brief register the 'information' global variable with QML
/// @note  see https://doc.qt.io/qt-6/qml-singleton.html#exposing-an-existing-object-as-a-singleton

struct DataTableModelForeign
{
  Q_GADGET
  QML_FOREIGN(DataTableModel)
  QML_SINGLETON
  QML_NAMED_ELEMENT(DataTableModel)

public:

  static DataTableModel *create(QQmlEngine *, QJSEngine *engine)
  {
    // The engine has to have the same thread affinity as the singleton.
    Q_ASSERT(engine->thread() == dataTableModel.thread());

    // There can only be one engine accessing the singleton.
    if (s_engine)
      Q_ASSERT(engine == s_engine);
    else
      s_engine = engine;

    // Explicitly specify C++ ownership so that the engine doesn't delete
    // the instance.
    QJSEngine::setObjectOwnership(&dataTableModel,
                                  QJSEngine::CppOwnership);
    return &dataTableModel;
  }

private:
  inline static QJSEngine *s_engine = nullptr;
};

} // namespace zg
