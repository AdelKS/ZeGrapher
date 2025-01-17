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

#include <QtQmlIntegration>
#include <QObject>

#include "MathObjects/expr.h"
#include "Utils/plotstyle.h"
#include "constant.h"
#include "equation.h"
#include "parametric.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(PlotStyle* style MEMBER style)

public:

  explicit MathObject(QObject *parent = nullptr);
  ~MathObject();

  Q_INVOKABLE void setBackend(mathobj::Constant*);
  Q_INVOKABLE void setBackend(mathobj::Equation*);
  Q_INVOKABLE void setBackend(mathobj::Expr*);
  Q_INVOKABLE void setBackend(mathobj::Parametric*);

  void setSlot(size_t slot);
  std::optional<size_t> get_slot() const { return slot; }

  /// @brief forwards the refresh() call to the current active backend
  void refresh();

  QStringList handledMathObjects() const;

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
  T* getBackend();

  template <class T>
  const T* getBackend() const;

  PlotStyle* style = nullptr;

protected:
  std::variant<
    std::monostate,
    mathobj::Equation*,
    mathobj::Expr*,
    mathobj::Constant*,
    mathobj::Parametric*
  > backend;

  std::optional<size_t> slot;
};

template <class T>
const T* MathObject::getBackend() const
{
  if (std::holds_alternative<T*>(backend))
    return std::get<T*>(backend);
  else return nullptr;
}

template <class T>
T* MathObject::getBackend()
{
  return const_cast<T*>(std::as_const(*this).getBackend<T>());
}

}
