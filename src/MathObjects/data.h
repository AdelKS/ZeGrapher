#pragma once

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

#include "BuildingBlocks/base.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

#include <string>
#include <vector>

namespace zg {

namespace mathobj {

/// @brief a DataSheet column: one expression per row, read at the row number
struct Data: Base, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name NOTIFY nameChanged)
  Q_PROPERTY(zg::State state READ getState WRITE setState NOTIFY stateChanged)
  Q_PROPERTY(bool showInTable WRITE setShowInTable MEMBER showInTable NOTIFY showInTableChanged )

public:

  explicit Data(QObject *parent = nullptr);
  ~Data();

  Q_INVOKABLE void setName(QString name);
  QString getName() const { return input_name; }

  void setState(State s);

  /// @brief the expression of each row, empty for an empty cell
  const std::vector<std::string>& getData() const;

  /// @brief replaces every value, between dataAboutToBeReplaced() and
  ///        dataReplaced()
  void setData(std::vector<std::string> values);

  /// @brief single-row edits, which DataTableModel makes and announces to its
  ///        views itself
  void setValue(size_t row, std::string value);
  void insertValues(size_t row, size_t count);
  void removeValue(size_t row);

  State sync();
  bool isValid() const { return state.isValid(); }

  const State& getState() const { return state; }

  Revision getRevision() const { return {std::pair(&zcMathObj, zcMathObj.get_revision())}; }
  EvalHandle getZcObject() const { return &zcMathObj; }

  void setShowInTable(bool);

  struct POD {
    std::optional<std::string> name;
    std::optional<std::string> start;
    std::optional<std::string> end;
    std::optional<CoordinateSystem> coordinates;
    std::optional<std::vector<std::string>> values;
    std::optional<PlotStyle::POD> style;
    std::optional<bool> show_in_table;

    auto operator<=>(const POD&) const = default;
  };

  POD exportPod() const;
  void importPod(POD);

public slots:
  void requestUiInitiatedDelete();

signals:
  void dataAboutToBeReplaced();
  void dataReplaced();
  void nameChanged();
  void stateChanged();
  void showInTableChanged();

protected:
  void valuesEdited();

  QString input_name;
  State state;
  bool showInTable = false;

};

}
}
