#include "parametric.h"
#include "zcmathobject.h"

namespace zg {

Parametric::Parametric(QObject *parent)
  : QObject(parent), obj1(new ZcMathObject(this)), obj2(new ZcMathObject(this))
{
  obj1->setBackend(ZcMathObject::NAMEDREF);
  obj2->setBackend(ZcMathObject::NAMEDREF);
}

void Parametric::sync()
{
  obj1->sync();
  obj2->sync();
}

} // namespace zg
