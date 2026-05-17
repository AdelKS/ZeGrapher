#include "parametric.h"

namespace zg {
namespace mathobj {

Parametric::Parametric(QObject *parent)
  : QObject(parent), obj1(new NamedRef(this)), obj2(new NamedRef(this))
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

  return obj1->getState();
}

YAML::Emitter& operator << (YAML::Emitter& o, const Parametric& p)
{
  o << YAML::Key << "first";
  o << YAML::Value << p.obj1->getName().toStdString();
  o << YAML::Key << "second";
  o << YAML::Value << p.obj2->getName().toStdString();
  return o;
}

} // namespace mathobj
} // namespace zg
