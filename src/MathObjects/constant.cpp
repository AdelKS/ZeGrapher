#include "MathObjects/constant.h"
#include "information.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): Stateful(parent)
{
  zcMathObj = std::nan("");
}

State Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return sync();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  emit updated();

  return sync();
}

bool Constant::isValid() const
{
  return getState().isValid();
}

State Constant::sync() {
  setState(State().update(zcMathObj.name_status()));

  return getState();
};

void Constant::set_value(double val)
{
  zcMathObj = val;
  emit updated();
}

}
}
