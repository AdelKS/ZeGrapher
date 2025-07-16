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

#include "MathObjects/expr.h"
#include "equation.h"
#include "constant.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct ZcMathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QSyntaxHighlighter* highlighter MEMBER highlighter)
  Q_PROPERTY(State state READ getState WRITE setState NOTIFY stateChanged)

public:

  explicit ZcMathObject(QObject *parent = nullptr);

  Q_INVOKABLE void setBackend(mathobj::Constant*);
  Q_INVOKABLE void setBackend(mathobj::Equation*);
  Q_INVOKABLE void setBackend(mathobj::Expr*);

  Q_INVOKABLE State setExpression(QString);
  Q_INVOKABLE State getState() const;
  Q_INVOKABLE void setState(State);

  bool isContinuous() const;
  bool isDiscrete() const;

  bool isValid() const;

  void setSlot(size_t slot);
  std::optional<size_t> get_slot() const { return slot; }

  /// @returns the name of the currently active math object
  QString getName() const;

  /// @returns list of math object names this object directly uses
  QStringList directDependencies() const;

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
    requires (zc::utils::is_any_of<T, mathobj::Equation, mathobj::Expr, mathobj::Constant>)
  T* getBackend();

  template <class T>
    requires (zc::utils::is_any_of<T, mathobj::Equation, mathobj::Expr, mathobj::Constant>)
  const T* getBackend() const;

  const zc::DynMathObject<zc_t>* getZcObject() const;

  QSyntaxHighlighter* highlighter = nullptr;

public slots:
  /// @brief forwards the refresh() call to the current active backend
  State refresh();

signals:
  void stateChanged();

protected:
  std::variant<std::monostate, mathobj::Equation*, mathobj::Expr*, mathobj::Constant*> backend;
  std::optional<size_t> slot;
};

template <class T>
  requires (zc::utils::is_any_of<T, mathobj::Equation, mathobj::Expr, mathobj::Constant>)
const T* ZcMathObject::getBackend() const
{
  if (std::holds_alternative<T*>(backend))
    return std::get<T*>(backend);
  else return nullptr;
}

template <class T>
  requires (zc::utils::is_any_of<T, mathobj::Equation, mathobj::Expr, mathobj::Constant>)
T* ZcMathObject::getBackend()
{
  return const_cast<T*>(std::as_const(*this).getBackend<T>());
}

}
