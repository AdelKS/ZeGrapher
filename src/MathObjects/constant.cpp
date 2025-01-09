#include "MathObjects/constant.h"
#include "information.h"

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent)
  : shared::StateBB(parent), shared::ZcMathObjectBB()
{
  zcMathObj = std::nan("");
}

void Constant::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
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

  if ((not oldName.isEmpty() or not name.isEmpty()) and oldName != name and slot)
    information.mathObjectUpdated(*slot, oldName, name);
}

void Constant::set_value(double val)
{
  zcMathObj = val;
  if (not name.isEmpty() and slot)
    information.mathObjectUpdated(*slot, name, name);
}

}
}
