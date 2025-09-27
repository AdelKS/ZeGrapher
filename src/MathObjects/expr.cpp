#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : Stateful(parent), shared::ZcMathObjectBB()
{
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

  qDebug() << "Setting implicit name: " << implicitName;

  auto name_status = zcMathObj.name_status();
  while (not name_status and name_status.error().type == zc::Error::NAME_ALREADY_TAKEN)
  {
    qDebug() << "implicit name taken: " << implicitName;
    implicitName = name + QString::number(i++);
    qDebug() << "Setting new implicit name: " << implicitName;
    zcMathObj.set_name(implicitName.toStdString());
    name_status = zcMathObj.name_status();
  }

  if (implicitName != name)
    emit implicitNameChanged();

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
    return sync();

  fullExpression = fullExpr;

  qDebug() << "[backend] Expr: setting expression: " << fullExpression;

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
  setState(State().update(zcMathObj.status()));

  updateValue();

  return getState();
}

} // namespace mathobj
} // namespace zg
