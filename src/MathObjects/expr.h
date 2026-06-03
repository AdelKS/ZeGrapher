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

#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {
namespace mathobj {

/// @brief Contains the information needed to compute the math object and how to plot it
struct Expr : QObject, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(double value READ getValue NOTIFY valueChanged)
  Q_PROPERTY(QString expression WRITE setExpression READ getExpression NOTIFY expressionChanged)
  Q_PROPERTY(QString implicitName WRITE setImplicitName MEMBER implicitName NOTIFY implicitNameChanged)
  Q_PROPERTY(bool schrodinger READ isSchrodinger NOTIFY schrodingerChanged)
  Q_PROPERTY(State state READ getState WRITE setState NOTIFY stateChanged)

public:

  explicit Expr(QObject *parent = nullptr);
  ~Expr();

  Q_INVOKABLE State setExpression(QString);
  Q_INVOKABLE State setImplicitName(QString name);
  void setState(State s);

  State sync();
  Q_INVOKABLE double getValue();
  Q_INVOKABLE QString getExpression() const { return expression; }
  Q_INVOKABLE bool isValid();
  const State& getState() const { return state; }

  QString getName() const { return implicitName; };

  void updateValue();

  void setSchrodinger(bool);
  Q_INVOKABLE bool isSchrodinger() const { return schrodinger; }

signals:
  void valueChanged();
  void implicitNameChanged();
  void updated();
  void schrodingerChanged();
  void expressionChanged();
  void stateChanged();

protected:
  QString expression;
  std::string fullExpression;
  State state;

  /// @brief the implicit variable that gets
  ///        defined in ZeCalculator for this expression
  /// @example an expression "2" with 'hidden_var_name' = 'xmin'
  ///          will be forwarded to ZeCalculator as "xmin = 2"
  QString implicitName = {};
  double value = std::nan("");
  bool schrodinger = false;

};

}
}
