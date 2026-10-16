#include "namedref.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "information.h"

namespace zg {
namespace mathobj {

NamedRef::NamedRef(QObject *parent): QObject(parent)
{
}

State NamedRef::setName(QString new_input_name)
{
  if (new_input_name == input_name)
    return sync();

  input_name = new_input_name;

  sync();

  emit updated();
  emit nameChanged();

  return getState();
}

void NamedRef::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

bool NamedRef::isValid()
{
  sync();
  return getState().isValid();
}

State NamedRef::sync()
{
  State newState;
  if (not input_name.isEmpty())
  {
    const auto* zcObj = getZcObject();
    if (zcObj)
    {
      if (*zcObj)
        newState.setValid();
      else newState.setInvalid(QObject::tr("Object \"%1\" is in an invalid state").arg(input_name));
    }
    else newState.setInvalid(QObject::tr("Object not defined"));
  }

  setState(newState);

  return getState();
};

bool NamedRef::isContinuous() const
{
  const auto* zcMathObj = getZcObject();
  return zcMathObj and *zcMathObj and not shared::holdsDiscrete(*zcMathObj);
}

bool NamedRef::isDiscrete() const
{
  const auto* zcMathObj = getZcObject();
  return zcMathObj and *zcMathObj and shared::holdsDiscrete(*zcMathObj);
}

const zc::DynMathObject<zc_t>* NamedRef::getZcObject() const
{
  return zg::mathWorld->backend.get(input_name.toStdString());
}

} // namespace mathobj
} // namespace zg
