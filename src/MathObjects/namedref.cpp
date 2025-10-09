#include "namedref.h"
#include "mathworld.h"

namespace zg {
namespace mathobj {

NamedRef::NamedRef(QObject *parent): Stateful(parent)
{
}

State NamedRef::setName(QString new_input_name)
{
  if (new_input_name == input_name)
    return sync();

  input_name = new_input_name;

  sync();

  emit updated();

  return getState();
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
      else newState.setInvalid(tr("Math object in invalid state."));
    }
    else newState.setInvalid(tr("Math object doesn't exist."));
  }

  setState(newState);

  return getState();
};

bool NamedRef::isContinuous() const
{
  if (const auto* zcMathObj = getZcObject(); zcMathObj and *zcMathObj)
    return not(zcMathObj->holds(zc::ObjectType::DATA) or zcMathObj->holds(zc::ObjectType::SEQUENCE));
  else return false;
}

bool NamedRef::isDiscrete() const
{
  if (const auto* zcMathObj = getZcObject(); zcMathObj and *zcMathObj)
    return zcMathObj->holds(zc::ObjectType::DATA) or zcMathObj->holds(zc::ObjectType::SEQUENCE);
  else return false;
}

const zc::DynMathObject<zc_t>* NamedRef::getZcObject() const
{
  return zc::mathWorld.get(input_name.toStdString());
}

} // namespace mathobj
} // namespace zg
