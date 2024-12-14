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
#include "zc.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(PlotStyle* style MEMBER style)

public:

  enum Type {ZC, Constant};
  Q_ENUM(Type)

  explicit MathObject(QObject *parent = nullptr);
  ~MathObject();

  Q_INVOKABLE void setBackend(mathobj::ZC*);
  Q_INVOKABLE void setBackend(mathobj::Expr*);

  /// @brief forwards the refresh() call to the current active backend
  void refresh();

  QStringList handledMathObjects() const;

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
    requires (zc::utils::is_any_of<T, mathobj::ZC, mathobj::Expr>)
  T* getBackend();

  template <class T>
    requires (zc::utils::is_any_of<T, mathobj::ZC, mathobj::Expr>)
  const T* getBackend() const;

  PlotStyle* style = nullptr;
  std::variant<std::monostate, mathobj::ZC*, mathobj::Expr*> backend;
};

template <class T>
  requires (zc::utils::is_any_of<T, mathobj::ZC, mathobj::Expr>)
const T* MathObject::getBackend() const
{
  if (std::holds_alternative<T*>(backend))
    return std::get<T*>(backend);
  else return nullptr;
}

template <class T>
  requires (zc::utils::is_any_of<T, mathobj::ZC, mathobj::Expr>)
T* MathObject::getBackend()
{
  return const_cast<T*>(std::as_const(*this).getBackend<T>());
}

}
