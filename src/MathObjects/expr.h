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

#include "BuildingBlocks/stateful.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {
namespace mathobj {

/// @brief Contains the information needed to compute the math object and how to plot it
struct Expr : Stateful, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(double value READ getValue NOTIFY valueChanged)
  Q_PROPERTY(QString expression WRITE setExpression READ getExpression)
  Q_PROPERTY(QString implicitName WRITE setImplicitName MEMBER implicitName NOTIFY implicitNameChanged)

public:

  explicit Expr(QObject *parent = nullptr);

  Q_INVOKABLE State setExpression(QString);
  Q_INVOKABLE State setImplicitName(QString name);
  State sync();
  Q_INVOKABLE double getValue();
  Q_INVOKABLE QString getExpression() const { return expression; }
  Q_INVOKABLE bool isValid();

  QString getName() const { return implicitName; };

  void updateValue();

signals:
  void valueChanged();
  void implicitNameChanged();
  void updated();

protected:
  QString expression;
  std::string fullExpression;

  /// @brief the implicit variable that gets
  ///        defined in ZeCalculator for this expression
  /// @example an expression "2" with 'hidden_var_name' = 'xmin'
  ///          will be forwarded to ZeCalculator as "xmin = 2"
  QString implicitName = {};
  double value = std::nan("");
};

}
}
