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

#include "zcbase.h"

namespace zg {
namespace mathobj {

/// @brief Contains the information needed to compute the math object and how to plot it
struct Expr : ZcBase {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(double value MEMBER value NOTIFY valueChanged)
  Q_PROPERTY(QString expression WRITE setExpression MEMBER expression)
  Q_PROPERTY(QString implicitName WRITE setImplicitName MEMBER implicitName)

public:

  explicit Expr(QObject *parent = nullptr);

  Q_INVOKABLE void refresh();
  Q_INVOKABLE void setExpression(QString expr);

  Q_INVOKABLE void setImplicitName(QString name);

  QString getImplicitName() const { return implicitName; };

signals:
  void valueChanged();

protected:
  QString expression = {};

  /// @brief the implicit variable that gets
  ///        defined in ZeCalculator for this expression
  /// @example an expression "2" with 'hidden_var_name' = 'xmin'
  ///          will be forwarded to ZeCalculator as "xmin = 2"
  QString implicitName = {};
  double value = std::nan("");
};

}
}
