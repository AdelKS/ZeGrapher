#include <string>

#include "MathObjects/data.h"
#include "Utils/yaml.h"

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

void Data::setName(QString new_input_name)
{
  if (input_name == new_input_name)
  {
    sync();
    return;
  }

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  sync();

  emit nameChanged();
  emit updated();
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

const std::vector<std::string>& Data::getData() const
{
  static const std::vector<std::string> empty;
  if (auto* ptr = zcMathObj.get_data())
    return *ptr;
  else return empty;
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

Data::POD Data::exportPod() const
{
  return POD {
    .name = yml::not_default(getName()),
    .start = yml::not_default(getStartStr(), getDefaultStringRange().start),
    .end = yml::not_default(getEndStr(), getDefaultStringRange().end),
    .coordinates = yml::not_default(coordinateSystem, Cartesian),
    .values = yml::not_default(getData()),
    .style = style.exportPod()
  };
}

}
}
