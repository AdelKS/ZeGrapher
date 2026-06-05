#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : Stateful(parent), shared::ZcMathObjectBB(),
    base([](CoordinateSystem s, bool isDiscrete)
          { return s == CoordinateSystem::Polar
                   ? isDiscrete
                     ? StringRange{"0", "10"}
                     : StringRange{"0", "2*math::pi"}
                   : isDiscrete
                     ? StringRange{"0",    "xmax"}
                     : StringRange{"xmin", "xmax"}; }, this)
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
  setState(State::from(zcMathObj.status()));

  return getState();
};

}
}
