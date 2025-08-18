#include "parametric.h"

namespace zg {
namespace mathobj {

Parametric::Parametric(QObject *parent)
  : QObject(parent), obj1(new NamedRef(this)), obj2(new NamedRef(this))
{
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

} // namespace mathobj
} // namespace zg
