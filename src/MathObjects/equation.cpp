#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : Stateful(parent), shared::ZcMathObjectBB()
{}

State Equation::setEquation(QString eq)
{
  if (eq == equation)
    return sync();

  equation = eq;
  zcMathObj = equation.toStdString();

  sync();

  emit updated();

  return getState();
}

bool Equation::isValid()
{
  sync();
  return getState().isValid();
}

QString Equation::getName() const
{
  return QString::fromStdString(std::string(zcMathObj.get_name()));
}

State Equation::sync()
{
  setState(State().update(zcMathObj.status()));

  return getState();
};

}
}
