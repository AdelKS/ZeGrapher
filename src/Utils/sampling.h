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

#include "MathObjects/data.h"
#include "MathObjects/namedref.h"

namespace zg {
namespace mathobj {

/// @brief Fills a DataSheet column with the values of a math object, taken
///        from 'start' to 'end' every 'step'
struct Sampling: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(Data* column READ getColumnPtr WRITE setColumn REQUIRED)
  Q_PROPERTY(NamedRef* source READ getSourcePtr CONSTANT)
  Q_PROPERTY(Expr* start READ getStartPtr CONSTANT)
  Q_PROPERTY(Expr* end READ getEndPtr CONSTANT)
  Q_PROPERTY(Expr* step READ getStepPtr CONSTANT)

public:

  explicit Sampling(QObject *parent = nullptr);

  static constexpr int maxPoints = 10'000'000;

  /// @brief takes a numerical snapshot of the inputs, and on success fills the column
  ///        with the sampled values, replacing what it held
  /// @returns error message, empty if the Data object got filled
  Q_INVOKABLE QString apply();

  void setColumn(Data*);

  NamedRef* getSourcePtr() { return &source; }
  Data* getColumnPtr() { return column; }
  Expr* getStartPtr() { return &start; }
  Expr* getEndPtr() { return &end; }
  Expr* getStepPtr() { return &step; }

protected:
  NamedRef source;
  Expr start, end, step;

  /// @brief owned by its DataSheet, we only write to it
  Data* column = nullptr;
};

}
}
