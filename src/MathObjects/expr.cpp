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

#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : QObject(parent), shared::ZcMathObjectBB()
{
  mathWorld->trackExprObject(this);
}

Expr::~Expr()
{
  mathWorld->untrackExprObject(this);
}

void Expr::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

State Expr::setImplicitName(QString name)
{
  static size_t internalCount = 0;
  if (name.startsWith(internalPrefix))
    name += '_' + QString::number(internalCount++);

  if (name != implicitName)
  {
    implicitName = name;
    zcMathObj.set_name(implicitName.toStdString());
    emit implicitNameChanged();
  }

  return setExpression(expression);
}

double Expr::evaluate() const
{
  const std::expected<double, zc::Error> res = zcMathObj({}, &zg::mathWorld->cache);
  return res ? *res : std::nan("");
}

void Expr::updateValue()
{
  const double old_value = value;
  value = evaluate();

  if (std::isnan(old_value) != std::isnan(value)
      or (not std::isnan(old_value) and not std::isnan(value) and old_value != value))
    emit valueChanged();
}

State Expr::setExpression(QString expr)
{
  if (expr != expression)
  {
    expression = expr;
    emit expressionChanged();
  }

  std::string fullExpr = implicitName.toStdString() + "=" + expr.toStdString();

  if (fullExpression == fullExpr)
  {
    updateValue();
    return sync();
  }

  fullExpression = fullExpr;

  if (not expression.isEmpty())
    zcMathObj = fullExpression;
  else zcMathObj = "";

  updateValue();
  sync();

  emit updated();

  return getState();
}

State Expr::sync()
{
  setState(State::from(zcMathObj.status()));

  updateValue();

  return getState();
}

void Expr::setSchrodinger(bool s)
{
  if (schrodinger == s)
    return;

  schrodinger = s;
  emit schrodingerChanged();
}

} // namespace mathobj
} // namespace zg
