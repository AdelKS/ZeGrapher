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
#include "Utils/plotstyle.h"
#include "namedref.h"

#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {

struct ZcMathObject;

/// @brief holds the names of the two objects used to plot a parametric equation
struct Parametric: Base {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(NamedRef* obj1 READ getObj1Ptr)
  Q_PROPERTY(NamedRef* obj2 READ getObj2Ptr)

public:

  explicit Parametric(QObject *parent = nullptr);

  State sync();

  bool isValid();

  bool isDiscrete() const;
  bool isContinuous() const;

  NamedRef* getObj1Ptr() { return &obj1; }
  NamedRef* getObj2Ptr() { return &obj2; }

  NamedRef obj1;
  NamedRef obj2;

  PlotStyle style;

  void setFirst(QString name) { obj1.setName(name); }
  QString getFirst() const { return obj1.getName(); }

  void setSecond(QString name) { obj2.setName(name); }
  QString getSecond() const { return obj2.getName(); }

  struct POD {
    std::optional<std::string> first;
    std::optional<std::string> second;
    std::optional<std::string> start;
    std::optional<std::string> end;
    std::optional<CoordinateSystem> coordinates;
    std::optional<PlotStyle::POD> style;

    auto operator<=>(const POD&) const = default;
  };

  POD exportPod() const;
  void importPod(POD);

signals:
  void updated();

};

} // namespace mathobj
} // namespace zg
