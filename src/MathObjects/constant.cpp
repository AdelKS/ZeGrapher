#include "MathObjects/constant.h"
#include "information.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent)
  : shared::StateBB(parent), shared::ZcMathObjectBB()
{
  zcMathObj = std::nan("");
}

void Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return;

  input_name = new_input_name;

  refresh();
}

void Constant::refresh()
{
  QString oldName = name;

  zcMathObj.set_name(input_name.toStdString());

  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  if (state)
    state->update(zcMathObj.name_status());

  if ((not oldName.isEmpty() or not name.isEmpty()) and oldName != name)
    information.mathObjectUpdated(oldName, name);
}

void Constant::set_value(double val)
{
  zcMathObj = val;
  if (not name.isEmpty())
    information.mathObjectUpdated(name, name);
}

}
}
