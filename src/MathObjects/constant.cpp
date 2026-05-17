#include "MathObjects/constant.h"
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

bool Constant::isValid()
{
  sync();
  return getState().isValid();
}

State Constant::sync() {
  setState(State::from(zcMathObj.name_status()));

  return getState();
};

void Constant::set_value(double val)
{
  if (value == val)
    return;

  zcMathObj = val;
  value = val;

  emit valueChanged();
  emit updated();
}

YAML::Emitter& operator << (YAML::Emitter& o, const Constant& c)
{
  o << YAML::Key << "name";
  o << YAML::Value << c.getName().toStdString();
  o << YAML::Key << "value";
  o << YAML::Value << c.get_value();
  return o;
}

}
}
