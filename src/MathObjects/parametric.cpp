#include "parametric.h"
#include "zcmathobject.h"

namespace zg {

Parametric::Parametric(QObject *parent)
  : QObject(parent)
{}

void Parametric::sync()
{
  Q_ASSERT(obj1 and obj2);

  obj1->sync();
  obj2->sync();
}

void Parametric::setSlot(size_t slot)
{
  if (obj1)
    obj1->setSlot(slot);

  if (obj2)
    obj2->setSlot(slot);
}

} // namespace zg
