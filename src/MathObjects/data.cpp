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

bool Data::isValid()
{
  sync();
  return getState().isValid();
}

State Data::sync() {
  setState(State::from(zcMathObj.name_status()));

  return getState();
};

YAML::Emitter& operator << (YAML::Emitter& o, const Data& d)
{
  o << YAML::Key << "name";
  o << YAML::Value << d.getName().toStdString();
  o << YAML::Key << "values";
  o << YAML::Flow;
  o << YAML::BeginSeq;

  for (size_t i = 0 ; i != d.zcMathObj.get_data_size() ; i++)
    if (auto opt_str = d.zcMathObj.get_data_point(i))
      o << *opt_str;
    else o << "";

  o << YAML::EndSeq;
  return o;
}

}
}
