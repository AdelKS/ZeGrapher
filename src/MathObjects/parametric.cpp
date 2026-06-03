#include "parametric.h"

namespace zg {
namespace mathobj {

Parametric::Parametric(QObject *parent)
  : zg::Base([](CoordinateSystem, bool){ return StringRange{"0", "10"}; }, parent),
    obj1(new NamedRef(this)),
    obj2(new NamedRef(this))
{
  connect(obj1, &NamedRef::updated, this, &Parametric::updated);
  connect(obj2, &NamedRef::updated, this, &Parametric::updated);
}

State Parametric::sync()
{
  obj1->sync();
  obj2->sync();

  if (obj1->getState().getStatus() == State::INVALID)
    return obj1->getState();
  if (obj2->getState().getStatus() == State::INVALID or obj2->getState().getStatus() == State::NEUTRAL)
    return obj2->getState();

  Base::setDiscrete(obj1->isDiscrete() or obj2->isDiscrete());

  return obj1->getState();
}

bool Parametric::isDiscrete() const
{
  return obj1->isDiscrete() or obj2->isDiscrete();
}

bool Parametric::isContinuous() const
{
  return obj1->isContinuous() and obj2->isContinuous();
}

} // namespace mathobj
} // namespace zg
