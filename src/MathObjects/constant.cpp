#include "MathObjects/constant.h"
#include "information.h"

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
    return state;

  input_name = new_input_name;

  return refresh();
}

State Constant::refresh()
{
  QString oldName = name;

  zcMathObj.set_name(input_name.toStdString());

  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  if ((not oldName.isEmpty() or not name.isEmpty()) and oldName != name and slot)
    information.mathObjectUpdated(*slot, oldName, name);

  state.update(zcMathObj.name_status());

  return state;
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

State Constant::getState() const { return state; };
void Constant::setState(State newState) { state = newState; }

void Constant::set_value(double val)
{
  zcMathObj = val;
  if (not name.isEmpty() and slot)
    information.mathObjectUpdated(*slot, name, name);
}

}
}
