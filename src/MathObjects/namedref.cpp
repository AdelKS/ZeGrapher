#include "namedref.h"
#include "mathworld.h"

namespace zg {
namespace mathobj {

NamedRef::NamedRef(QObject *parent): Highlighted(parent)
{
}

State NamedRef::setName(QString new_input_name)
{
  if (new_input_name == input_name)
    return sync();

  input_name = new_input_name;

  sync();

  emit updated();

  return state;
}

bool NamedRef::isValid() const
{
  return state.isValid();
}

State NamedRef::setExpression(QString name)
{
  doNotRehighlight = true;
  setName(name);
  doNotRehighlight = false;
  return state;
}

State NamedRef::sync()
{
  State oldState = state;
  if (not input_name.isEmpty())
  {
    const auto* zcObj = getZcObject();
    if (zcObj)
    {
      if (*zcObj)
        state.setValid();
      else state.setInvalid(tr("Math object in invalid state."));
    }
    else state.setInvalid(tr("Math object doesn't exist."));
  }

  if (state != oldState)
  {
    if (highlighter and not doNotRehighlight)
      highlighter->rehighlight();

    emit stateChanged();
  }

  return state;
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
