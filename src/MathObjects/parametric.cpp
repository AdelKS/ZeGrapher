#include "parametric.h"
#include "zcmathobject.h"

namespace zg {

Parametric::Parametric(QObject *parent)
  : QObject(parent)
{}

State Parametric::refresh()
{
  Q_ASSERT(obj1 and obj2);

  State state1 = obj1->refresh();
  State state2 = obj2->refresh();
  if (not state1.isValid())
    return state1;
  if (not state2.isValid())
    return state2;
  return state1;
}

void Parametric::setSlot(size_t slot)
{
  if (obj1)
    obj1->setSlot(slot);

  if (obj2)
    obj2->setSlot(slot);
}

} // namespace zg
