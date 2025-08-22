#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : Highlighted(parent), shared::ZcMathObjectBB()
{}

State Equation::setEquation(QString eq)
{
  if (eq == equation)
    return sync();

  equation = eq;
  zcMathObj = equation.toStdString();

  sync();

  emit updated();

  return state;
}

bool Equation::isValid() const
{
  return state.isValid();
}

QString Equation::getName() const
{
  return QString::fromStdString(std::string(zcMathObj.get_name()));
}

State Equation::setExpression(QString expr)
{
  doNotRehighlight = true;
  setEquation(expr);
  doNotRehighlight = false;
  return state;
}

State Equation::sync()
{
  State oldState = state;
  state.update(zcMathObj.status());

  if (state != oldState)
  {
    if (highlighter and not doNotRehighlight)
      highlighter->rehighlight();

    emit stateChanged();
  }

  return state;
};

}
}
