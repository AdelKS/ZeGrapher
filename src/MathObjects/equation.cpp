#include "MathObjects/equation.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : QObject(parent), shared::ZcMathObjectBB()
{}

State Equation::setEquation(QString eq)
{
  if (eq == equation)
    return getState();

  equation = eq;
  zcMathObj = equation.toStdString();

  return getState();
}

bool Equation::isValid() const
{
  return getState().isValid();
}

void Equation::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

QString Equation::getName() const
{
  return QString::fromStdString(std::string(zcMathObj.get_name()));
}

State Equation::getState() const
{
  State state;
  state.update(zcMathObj.status());
  return state;
};

}
}
