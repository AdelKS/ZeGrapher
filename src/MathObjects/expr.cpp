#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : QObject(parent), shared::ZcMathObjectBB()
{
  setImplicitName("init");
}

void Expr::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

State Expr::setImplicitName(QString name)
{
  implicitName = std::move(name);
  while (information.getMathWorld().contains(implicitName.toStdString()))
  {
    qDebug() << "[backend] Expr: variable name '" << implicitName << "' already taken. Appending 'z' to it.";
    implicitName.push_back('z');
  }
  return setExpression(expression);
}

State Expr::setExpression(QString expr)
{
  if (expr == expression)
    return state;

  expression = std::move(expr);
  std::string full_expression = implicitName.toStdString() + "=" + expression.toStdString();
  qDebug() << "[backend] Expr: setting expression: " << full_expression;

  if (not expression.isEmpty())
    zcMathObj = full_expression;
  else zcMathObj = "";
  refresh();

  if (slot)
    information.mathObjectUpdated(*slot, implicitName, implicitName);

  return state;
}

State Expr::getState() const { return state; }

void Expr::setState(State newState) { state = newState; }

State Expr::refresh()
{
  std::string full_expression = implicitName.toStdString() + "=" + expression.toStdString();
  qDebug() << "[backend] Expr: refreshing evaluation of expression: " << full_expression;
  double oldValue = value;

  auto old_state = state;

  if (zcMathObj.has_value())
  {
    auto exp_val = zcMathObj();
    if (exp_val)
      value = *exp_val;
    state.update(exp_val);
  }
  else state.update(zcMathObj.status());

  if (oldValue != value or old_state != state)
  {
    qDebug() << "[backend] Expr: new value: " << full_expression << "=" << value;
    emit valueChanged();
  }

  return state;
}

}
}
