#include "MathObjects/equation.h"
#include "Utils/yaml.h"
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

void Equation::setEquation(QString eq)
{
  if (eq == equation)
  {
    sync();
    return;
  }

  equation = eq;
  zcMathObj = equation.toStdString();

  sync();

  emit equationChanged();
  emit updated();

  connect(&style, &PlotStyle::updated, this, &Equation::updated);
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

Equation::POD Equation::exportPod() const {

  return POD {
    .equation = yml::not_default(getEquation()),
    .start = yml::not_default(getStartStr(), getDefaultStringRange().start),
    .end = yml::not_default(getEndStr(), getDefaultStringRange().end),
    .coordinates = yml::not_default(coordinateSystem, Cartesian),
    .style = style.exportPod()
  };
}

void Equation::importPod(Equation::POD p)
{
  if (p.equation)
    setEquation(QString::fromStdString(*p.equation));

  if (p.start)
    setStart(QString::fromStdString(*p.start));

  if (p.end)
    setEnd(QString::fromStdString(*p.end));

  if (p.coordinates)
    setCoordinateSystem(*p.coordinates);

  if (p.style)
    style.importPod(std::move(*p.style));
}

}
}
