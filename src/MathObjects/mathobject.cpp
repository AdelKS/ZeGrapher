#include "MathObjects/zcmathobject.h"
#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{
  slot = information.registerMathObject(this);
}

void MathObject::setBackend(ZcMathObject* b)
{
  assert(b);
  backend = b;
  b->setSlot(slot);
}

void MathObject::setBackend(Parametric* p)
{
  assert(p);
  backend = p;
  p->setSlot(slot);
}

MathObject::EvalHandle MathObject::getZcObject() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const ZcMathObject* c) -> EvalHandle {
        return c->getZcObject();
      },
      [](const Parametric* p) -> EvalHandle {
        return std::make_pair(p->obj1->getZcObject(), p->obj2->getZcObject());
      },
      [](std::monostate) -> EvalHandle {
        return std::monostate{};
      },
    },
    backend
  );
}

bool MathObject::isContinuous() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const ZcMathObject* c) {
        return c->isContinuous();
      },
      [](const Parametric* p) {
        return p->obj1->isContinuous() and p->obj2->isContinuous();
      },
      [](std::monostate) {
        return false;
      },
    },
    backend
  );
}

bool MathObject::isDiscrete() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](const ZcMathObject* c) {
        return c->isDiscrete();
      },
      [](const Parametric* p) {
        return p->obj1->isDiscrete() or p->obj2->isDiscrete();
      },
      [](std::monostate) {
        return false;
      },
    },
    backend
  );
}

bool MathObject::isValid() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const ZcMathObject* e) {
        return e->getState().getStatus() == State::VALID;
      },
      [](const Parametric* p) {
        return p->obj1->isValid() and p->obj2->isValid();
      },
      [](std::monostate) { return false; },
    },
    backend
  );
}

QString MathObject::getName() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const ZcMathObject* e) {
        return e->getName();
      },
      [](const Parametric*) {
        return QString();
      },
      [](std::monostate) { return QString{}; },
    },
    backend
  );
}

void MathObject::updateMetadata()
{
  if (not style)
    return;

  if (isDiscrete())
    style->setObjectType(PlotStyle::Discrete);
  else if (isContinuous())
    style->setObjectType(PlotStyle::Continuous);
  else style->setObjectType(PlotStyle::NonRepresentable);
}

void MathObject::sync()
{
  std::visit(zc::utils::overloaded{
    [](std::monostate) {},
    [](auto* e) { e->sync(); },
  }, backend);
  updateMetadata();
}

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
