#include "MathObjects/constant.h"
#include "information.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): Highlighted(parent)
{
  zcMathObj = std::nan("");
}

State Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return sync();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  information.mathObjectUpdated();

  return sync();
}

bool Constant::isValid() const
{
  return state.isValid();
}

State Constant::setExpression(QString name)
{
  doNotRehighlight = true;
  setName(name);
  doNotRehighlight = false;
  return state;
}

State Constant::sync() {
  State oldState = state;
  state.update(zcMathObj.name_status());

  if (state != oldState)
  {
    if (highlighter and not doNotRehighlight)
      highlighter->rehighlight();

    emit stateChanged();
  }

  return state;
};

void Constant::set_value(double val)
{
  zcMathObj = val;
  information.mathObjectUpdated();
}

}
}
