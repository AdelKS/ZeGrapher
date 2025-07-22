#include "MathObjects/constant.h"
#include "information.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): QObject(parent)
{
  zcMathObj = std::nan("");
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

State Constant::getState() const {
  State state;
  state.update(zcMathObj.name_status());
  return state;
};

void Constant::set_value(double val)
{
  zcMathObj = val;
  information.mathObjectUpdated();
}

}
}
