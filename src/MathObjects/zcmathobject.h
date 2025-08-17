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
#include <QSyntaxHighlighter>

#include "constant.h"
#include "data.h"
#include "equation.h"
#include "expr.h"
#include "namedref.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct ZcMathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QSyntaxHighlighter* highlighter MEMBER highlighter)
  Q_PROPERTY(State state READ getState NOTIFY stateChanged)

public:

  enum BackendType {MONOSTATE, EQUATION, EXPR, CONSTANT, NAMEDREF, DATA};
  Q_ENUM(BackendType);

  explicit ZcMathObject(QObject *parent = nullptr);

  Q_INVOKABLE void setBackend(BackendType);

  Q_INVOKABLE State setExpression(QString);
  Q_INVOKABLE State getState() const;

  Q_INVOKABLE mathobj::Equation* getEquation();
  Q_INVOKABLE mathobj::Expr* getExpr();
  Q_INVOKABLE mathobj::Constant* getConstant();
  Q_INVOKABLE mathobj::NamedRef* getNamedRef();
  Q_INVOKABLE mathobj::Data* getData();

  bool isContinuous() const;
  bool isDiscrete() const;

  bool isValid() const;

  /// @returns the name of the currently active math object
  QString getName() const;

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
  T* getBackend();

  template <class T>
  const T* getBackend() const;

  const zc::DynMathObject<zc_t>* getZcObject() const;

  QSyntaxHighlighter* highlighter = nullptr;

public slots:
  /// @brief syncs with the backend
  State sync();

signals:
  void stateChanged();

protected:
  std::variant<std::monostate,
               mathobj::Equation*,
               mathobj::Expr*,
               mathobj::Constant*,
               mathobj::NamedRef*,
               mathobj::Data*>
    backend;

  State state;
};

template <class T>
const T* ZcMathObject::getBackend() const
{
  if (std::holds_alternative<T*>(backend))
    return std::get<T*>(backend);
  else return nullptr;
}

template <class T>
T* ZcMathObject::getBackend()
{
  return const_cast<T*>(std::as_const(*this).getBackend<T>());
}

}
