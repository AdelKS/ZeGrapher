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
