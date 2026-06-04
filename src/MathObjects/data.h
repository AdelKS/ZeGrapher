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
#include "Utils/plotstyle.h"
#include "Utils/state.h"

#include <string>
#include <vector>

namespace zg {
namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Data: Base, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name NOTIFY nameChanged)
  Q_PROPERTY(State state READ getState WRITE setState NOTIFY stateChanged)

public:

  explicit Data(QObject *parent = nullptr);

  Q_INVOKABLE void setName(QString name);
  QString getName() const { return input_name; }

  void setData(std::vector<std::string> values);
  void setState(State s);

  const std::vector<std::string>& getData() const;

  State sync();
  Q_INVOKABLE bool isValid();
  const State& getState() const { return state; }

  PlotStyle style;

  struct POD {
    std::optional<std::string> name;
    std::optional<std::string> start;
    std::optional<std::string> end;
    std::optional<CoordinateSystem> coordinates;
    std::optional<std::vector<std::string>> values;
    std::optional<PlotStyle::POD> style;
  };

  POD exportPod() const;

signals:
  void updated();
  void nameChanged();
  void stateChanged();

protected:
  QString input_name;
  State state;

};

}
}
