#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : Base(parent)
{
  setImplicitName("init");
}

void Expr::setImplicitName(QString name)
{
  implicitName = std::move(name);
  while (information.getMathWorld().contains(implicitName.toStdString()))
  {
    qDebug() << "[backend] Expr: variable name '" << implicitName << "' already taken. Appending 'z' to it.";
    implicitName.push_back('z');
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
    zcMathObj = zc::As<zc::Function<zc_t>>{full_expression};
  else zcMathObj = zc::As<zc::Function<zc_t>>{""};
  refresh();
  information.mathObjectUpdated(implicitName, implicitName);
}

void Expr::refresh()
{
  std::string full_expression = implicitName.toStdString() + "=" + expression.toStdString();
  qDebug() << "[backend] Expr: refreshing evaluation of expression: " << full_expression;
  double oldValue = value;

  if (zcMathObj.has_value())
  {
    auto exp_val = zcMathObj.value_as<zc::Function<zc_t>>().evaluate();
    if (exp_val)
      value = *exp_val;
    else
    {
      if (state)
        state->update(exp_val);
    }
  }
  else
  {
    if (state)
      state->update(zcMathObj.as_expected());
  }

  if (oldValue != value)
  {
    qDebug() << "[backend] Expr: new value: " << full_expression << "=" << value;
    if (state)
      state->setValid();
    emit valueChanged();
  }
}

}
}
