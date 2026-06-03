#include "MathObjects/data.h"
#include <string>

namespace zg {
namespace mathobj {

Data::Data(QObject *parent)
  : Base([](CoordinateSystem s, bool){
             return s == CoordinateSystem::Polar
             ? StringRange{"0", "10"}
             : StringRange{"0", "xmax"}; }, parent)
{
  zcMathObj.set_data(std::vector<std::string>(10));
}

State Data::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return sync();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  sync();

  emit nameChanged();
  emit updated();

  return getState();
}

void Data::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

void Data::setData(std::vector<std::string> values)
{
  zcMathObj.set_data(std::move(values));

  sync();

  emit updated();
}

bool Data::isValid()
{
  sync();
  return getState().isValid();
}

State Data::sync() {
  setState(State::from(zcMathObj.name_status()));

  Base::setDiscrete(shared::ZcMathObjectBB::isDiscrete());
  style.setDiscrete(Base::isDiscrete());

  return getState();
};

}
}
