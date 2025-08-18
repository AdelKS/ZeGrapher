#include "parametric.h"
#include "zcmathobject.h"

namespace zg {

Parametric::Parametric(QObject *parent)
  : QObject(parent), obj1(new ZcMathObject(this)), obj2(new ZcMathObject(this))
{
  obj1->setBackend(ZcMathObject::NAMEDREF);
  obj2->setBackend(ZcMathObject::NAMEDREF);
}

State Parametric::sync()
{
  obj1->sync();
  obj2->sync();

  if (obj1->getState().getStatus() == State::INVALID)
    return obj1->getState();
  if (obj2->getState().getStatus() == State::INVALID or obj2->getState().getStatus() == State::NEUTRAL)
    return obj2->getState();

  return obj1->getState();
}

} // namespace zg
