#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : Base([](CoordinateSystem s, bool isDiscrete)
          { return s == CoordinateSystem::Polar
                   ? isDiscrete
                     ? StringRange{"0", "10"}
                     : StringRange{"0", "2*math::pi"}
                   : isDiscrete
                     ? StringRange{"0",    "xmax"}
                     : StringRange{"xmin", "xmax"}; }, parent), shared::ZcMathObjectBB()
{}

State Equation::setEquation(QString eq)
{
  if (eq == equation)
    return sync();

  equation = eq;
  zcMathObj = equation.toStdString();

  sync();

  emit equationChanged();
  emit updated();

  return getState();
}

bool Equation::isValid()
{
  sync();
  return getState().isValid();
}

void Equation::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

QString Equation::getName() const
{
  return QString::fromStdString(std::string(zcMathObj.get_name()));
}

State Equation::sync()
{
  setState(State::from(zcMathObj.status()));

  Base::setDiscrete(shared::ZcMathObjectBB::isDiscrete());
  style.setDiscrete(Base::isDiscrete());

  return getState();
};

}
}
