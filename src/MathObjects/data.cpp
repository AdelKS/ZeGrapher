#include "MathObjects/data.h"
#include <string>

namespace zg {
namespace mathobj {

Data::Data(QObject *parent): Stateful(parent)
{
  zcMathObj.set_data("", std::vector<std::string>(10));
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

State Data::setData(QString name, std::vector<std::string> values)
{
  bool nameChange = input_name != name;
  input_name = name;
  zcMathObj.set_data(name.toStdString(), std::move(values));

  sync();

  if (nameChange)
    emit nameChanged();

  emit updated();

  return getState();
}

bool Data::isValid() const
{
  return getState().isValid();
}

State Data::sync() {
  setState(State().update(zcMathObj.name_status()));

  return getState();
};

}
}
