#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : shared::StateBB(parent), shared::ZcMathObjectBB()
{
  setImplicitName("hidden_variable_");
}

void Expr::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

void Expr::setImplicitName(QString name)
{
  implicitName = std::move(name);
  while (information.getMathWorld().contains(implicitName.toStdString()))
  {
    qDebug() << "[backend] Expr: variable name '" << implicitName << "' already taken. Appending 'z' to it.";
    implicitName.push_back('_');
  }
  setExpression(expression);
}

void Expr::setExpression(QString expr)
{
  if (expr == expression)
    return;

  expression = std::move(expr);
  std::string full_expression = implicitName.toStdString() + "=" + expression.toStdString();
  qDebug() << "[backend] Expr: setting expression: " << full_expression;

  if (not expression.isEmpty())
    zcMathObj = full_expression;
  else zcMathObj = "";
  refresh();

  if (slot)
    information.mathObjectUpdated(*slot, implicitName, implicitName);
}

void Expr::refresh()
{
  std::string full_expression = implicitName.toStdString() + "=" + expression.toStdString();
  qDebug() << "[backend] Expr: refreshing evaluation of expression: " << full_expression;
  double oldValue = value;

  auto old_opt_error = opt_error;

  if (zcMathObj.has_value())
  {
    auto exp_val = zcMathObj();
    if (exp_val)
      value = *exp_val;
    else opt_error = exp_val.error();
  }
  else opt_error = zcMathObj.error();

  if (oldValue != value or old_opt_error != opt_error)
  {
    qDebug() << "[backend] Expr: new value: " << full_expression << "=" << value;
    if (state)
      state->update(opt_error);
    emit valueChanged();
  }
}

}
}
