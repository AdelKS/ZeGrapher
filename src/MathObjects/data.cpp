#include "MathObjects/data.h"
#include "information.h"
#include <string>

namespace zg {
namespace mathobj {

Data::Data(QObject *parent): Highlighted(parent)
{
  zcMathObj.set_data("", std::vector<std::string>(10));
}

State Data::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return sync();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  information.mathObjectUpdated();

  return sync();
}

bool Data::isValid() const
{
  return state.isValid();
}

State Data::setExpression(QString name)
{
  doNotRehighlight = true;
  setName(name);
  doNotRehighlight = false;
  return state;
}

State Data::sync() {
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

}
}
