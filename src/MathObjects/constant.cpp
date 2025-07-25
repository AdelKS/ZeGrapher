#include "MathObjects/constant.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): QObject(parent)
{
  zcMathObj = std::nan("");
}

void Constant::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

State Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return getState();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  return getState();
}

bool Constant::isValid() const
{
  return getState().isValid();
}

zg::real_unit Constant::operator () () const
{
  return evaluate();
}

zg::real_unit Constant::evaluate() const
{
  tl::expected<double, zc::Error> exp_res = zcMathObj();
  if (exp_res)
    return zg::real_unit{*exp_res};
  else return zg::real_unit{std::nan("")};
}

State Constant::getState() const {
  State state;
  state.update(zcMathObj.name_status());
  return state;
};

void Constant::set_value(double val)
{
  zcMathObj = val;
}

}
}
