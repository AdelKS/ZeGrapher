#pragma once

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

#include "data.h"

#include <QAbstractListModel>

#include <vector>

namespace zg {

struct MathObject;

namespace mathobj {

/// @brief ZeGrapher math object that gathers Data objects as the columns of a table
/// @note  the columns are QObject children of the sheet, and MathWorld only
///        reaches them through it
struct DataSheet: QAbstractListModel {
  Q_OBJECT
  // 'DataTable' is taken by DataTable.qml
  QML_ELEMENT

  Q_PROPERTY(int columns READ rowCount NOTIFY columnsChanged)
  Q_PROPERTY(QString notes WRITE setNotes MEMBER notes NOTIFY notesChanged)

public:

  explicit DataSheet(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  const std::vector<Data*>& getColumns() const { return columns; }

  void setNotes(QString);

  Q_INVOKABLE zg::mathobj::Data* addColumn();

  Q_INVOKABLE void deleteColumn(zg::mathobj::Data*);
  Q_INVOKABLE void moveColumn(int from, int to);
  Q_INVOKABLE int indexOfColumn(const zg::mathobj::Data*) const;

  Q_INVOKABLE bool isValid() const;

  void sync();

  struct POD {
    std::optional<std::string> notes;
    std::optional<std::vector<Data::POD>> columns;

    auto operator<=>(const POD&) const = default;
  };

  POD exportPod() const;
  void importPod(POD);

signals:
  void updated();
  void columnsChanged();
  void notesChanged();

protected:
  /// @brief forgets a column without touching it, it may be halfway through destruction
  void forgetColumn(const zg::mathobj::Data*);

  std::vector<Data*> columns;

  /// @brief free-form comment the user attaches to the sheet, math ignores it
  QString notes;
};

}
}
