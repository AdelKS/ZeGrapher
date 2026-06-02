#include "parametric.h"
#include "Utils/yaml.h"

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
  style.setDiscrete(Base::isDiscrete());

  return obj1->getState();
}

bool Parametric::isValid() const
{
  return obj1->isValid() and obj2->isValid();
}

bool Parametric::isDiscrete() const
{
  return obj1->isDiscrete() or obj2->isDiscrete();
}

bool Parametric::isContinuous() const
{
  return obj1->isContinuous() and obj2->isContinuous();
}

Parametric::POD Parametric::exportPod() const {

  return POD {
    .first = yml::not_default(getFirst()),
    .second = yml::not_default(getSecond()),
    .start = yml::not_default(getStartStr(), getDefaultStringRange().start),
    .end = yml::not_default(getEndStr(), getDefaultStringRange().end),
    .coordinates = yml::not_default(coordinateSystem, Cartesian),
    .style = style.exportPod()
  };
}

void Parametric::importPod(Parametric::POD p)
{
  if (p.first)
    setFirst(QString::fromStdString(*p.first));

  if (p.second)
    setFirst(QString::fromStdString(*p.second));

  if (p.start)
    setStart(QString::fromStdString(*p.start));

  if (p.end)
    setStart(QString::fromStdString(*p.end));

  if (p.coordinates)
    setCoordinateSystem(*p.coordinates);
}

} // namespace mathobj
} // namespace zg
