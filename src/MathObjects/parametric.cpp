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

} // namespace zg
