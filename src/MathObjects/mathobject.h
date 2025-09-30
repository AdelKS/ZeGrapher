#pragma once

/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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
#include <QSyntaxHighlighter>

#include "Utils/plotstyle.h"
#include "parametric.h"
#include "equation.h"
#include "expr.h"
#include "constant.h"
#include "data.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
  Q_PROPERTY(Type type WRITE setType READ getType NOTIFY typeChanged)
  Q_PROPERTY(bool discrete READ isDiscrete NOTIFY continuityChanged)
  Q_PROPERTY(bool continuous READ isContinuous NOTIFY continuityChanged)

public:
  using EvalHandle
    = std::variant<std::monostate,
                   const zc::DynMathObject<zc_t>*,
                   std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*>>;

  enum Type
  {
    MONOSTATE,
    EQUATION,
    EXPR,
    CONSTANT,
    NAMEDREF,
    DATA,
    PARAMETRIC
  };

  explicit MathObject(QObject *parent = nullptr, Type type = EQUATION);

  Q_ENUM(Type);

  Q_INVOKABLE void setType(Type);
  Q_INVOKABLE Type getType() const;

  bool isValid() const { return state.isValid(); };
  QString getName() const { return name; };

  size_t getRevision() const;

  EvalHandle getZcObject() const;

  Q_INVOKABLE mathobj::Equation* getEquation();
  Q_INVOKABLE mathobj::Expr* getExpr();
  Q_INVOKABLE mathobj::Constant* getConstant();
  Q_INVOKABLE mathobj::NamedRef* getNamedRef();
  Q_INVOKABLE mathobj::Data* getData();
  Q_INVOKABLE mathobj::Parametric* getParametric();

  Q_INVOKABLE bool isContinuous() const { return continuous; };
  Q_INVOKABLE bool isDiscrete() const { return discrete; };

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
  T* getBackend();

  template <class T>
  const T* getBackend() const;

public slots:
  /// @brief forwards the refresh() call to the current active backend
  State sync();

signals:
  void stateChanged();
  void updated();
  void typeChanged();
  void continuityChanged();
  void nameChanged();

protected:
  std::variant<std::monostate,
               mathobj::Equation*,
               mathobj::Expr*,
               mathobj::Constant*,
               mathobj::NamedRef*,
               mathobj::Data*,
               mathobj::Parametric*> backend;
  QString name;
  State state;
  bool discrete = false;
  bool continuous = false;
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
