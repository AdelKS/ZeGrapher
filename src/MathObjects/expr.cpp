#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : QObject(parent), shared::ZcMathObjectBB()
{
  setImplicitName("hidden_variable");
}

bool Expr::isValid() const
{
  return getState().isValid();
}

State Expr::setImplicitName(QString name)
{
  size_t i = 1;
  implicitName = name;
  zcMathObj.set_name(implicitName.toStdString());

  auto name_status = zcMathObj.name_status();
  while (not name_status and name_status.error().type == zc::Error::NAME_ALREADY_TAKEN)
  {
    implicitName = name + QString::number(i++);
    zcMathObj.set_name(implicitName.toStdString());
    name_status = zcMathObj.name_status();
  }

  return setExpression(expression);
}

void Expr::updateValue()
{
  tl::expected<double, zc::Error> exp_res = zcMathObj({}, &information.mathObjectCache);
  double old_value = value;
  if (exp_res)
    value = zg::real_unit{*exp_res}.v;
  else value = std::nan("");

  if (old_value != value)
    emit valueChanged();
}

State Expr::setExpression(QString expr)
{
  expression = expr;

  std::string fullExpr = implicitName.toStdString() + "=" + expr.toStdString();

  if (fullExpression == fullExpr)
    return getState();

  fullExpression = fullExpr;

  qDebug() << "[backend] Expr: setting expression: " << fullExpression;

  if (not expression.isEmpty())
    zcMathObj = fullExpression;
  else zcMathObj = "";

  updateValue();

  return getState();
}

State Expr::getState() const
{
  State state;
  state.update(zcMathObj.status());
  return state;
}

} // namespace mathobj
} // namespace zg
