#include "expr.h"
#include "information.h"

namespace zg {
namespace mathobj {

Expr::Expr(QObject *parent)
  : ZcBase(parent)
{
  setImplicitName("init");
}

void Expr::setImplicitName(QString name)
{
  implicitName = std::move(name);
  while (information.getMathWorld().contains(implicitName.toStdString()))
  {
    qDebug() << "[backend] variable name '" << implicitName << "' already taken. Appending 'z' to it.";
    implicitName.push_back('z');
  }
  setExpression(expression);
}

void Expr::setExpression(QString expr)
{
  qDebug() << "[backend] parsing expression: " << implicitName + "=" + expr;
  if (not expr.isEmpty())
    zcMathObj = zc::As<zc::Function<zc_t>>{implicitName.toStdString() + "=" + expr.toStdString()};
  else zcMathObj = zc::As<zc::Function<zc_t>>{""};
  expression = std::move(expr);
  refresh();
  information.mathObjectUpdated(implicitName, implicitName);
}

void Expr::refresh()
{
  double oldValue = value;

  if (zcMathObj.has_value())
  {
    auto exp_val = zcMathObj.value_as<zc::Function<zc_t>>().evaluate();
    if (exp_val)
      value = *exp_val;

    if (state)
      state->update(exp_val);
  }
  else
  {
    if (state)
      state->update(zcMathObj.as_expected());
  }

  if (oldValue != value)
  {
    qDebug() << "[backend] " << implicitName << ": new value";
    emit valueChanged();
  }
}

}
}
