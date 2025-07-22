#include "namedref.h"
#include "information.h"

namespace zg {
namespace mathobj {

NamedRef::NamedRef(QObject *parent): QObject(parent)
{
}

State NamedRef::setName(QString new_input_name)
{
  if (new_input_name == input_name)
    return getState();

  input_name = new_input_name;

  return getState();
}

bool NamedRef::isValid() const
{
  return getState().isValid();
}

State NamedRef::getState() const
{
  State state;
  if (not input_name.isEmpty())
  {
    const auto* zcObj = getZcObject();
    if (zcObj)
    {
      if (*zcObj)
        state.setValid();
      else state.setInvalid(tr("Object is in an invalid state."));
    }
    else state.setInvalid(tr("Math object doesn't exist."));
  }

  return state;
};

const zc::DynMathObject<zc_t>* NamedRef::getZcObject() const
{
  return information.getMathWorld().get(input_name.toStdString());
}

} // namespace mathobj
} // namespace zg
