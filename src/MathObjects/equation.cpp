#include "MathObjects/equation.h"
#include "information.h"

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

  if (slot)
    information.mathObjectUpdated(*slot);

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

zg::real_unit Equation::operator () (zg::real_unit input, zc::eval::Cache* cache) const
{
  return evaluate(input, cache);
}

zg::real_unit Equation::evaluate(zg::real_unit input, zc::eval::Cache* cache) const
{
  tl::expected<double, zc::Error> exp_res = zcMathObj({input.v}, cache);
  if (exp_res)
    return zg::real_unit{*exp_res};
  else return zg::real_unit{std::nan("")};
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
