#include "MathObjects/data.h"

namespace zg {
namespace mathobj {

Data::Data(QObject *parent): QObject(parent)
{
  zcMathObj.set_data("", {});
}

State Data::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return getState();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  return getState();
}

bool Data::isValid() const
{
  return getState().isValid();
}

State Data::getState() const {
  State state;
  state.update(zcMathObj.name_status());
  return state;
};

}
}
