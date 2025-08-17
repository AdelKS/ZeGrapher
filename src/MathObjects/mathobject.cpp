#include "MathObjects/zcmathobject.h"
#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{
}

void MathObject::setBackend(MathObject::BackendType t)
{
  // we assume here that the order of the enum is the same as in the variant alternatives
  // if the index is different, delete the pointer, Qt style memory management
  if (backend.index() != size_t(t))
  {
    std::visit(
      zc::utils::overloaded{
        [](auto* n) {
          n->deleteLater();
        },
        [](std::monostate) {},
      },
      backend
    );
  }
  else return; // backend is already correct

  switch (t) {
    case MONOSTATE:
      backend = std::monostate{};
    break;
    case ZCMATHOBJECT:
      backend = new ZcMathObject(this);
    break;
    case PARAMETRIC:
      backend = new Parametric(this);
    break;
  }
}

ZcMathObject* MathObject::getZcMathObject()
{
  return getBackend<ZcMathObject>();
}

Parametric* MathObject::getParametric()
{
  return getBackend<Parametric>();
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

void MathObject::sync()
{
  std::visit(zc::utils::overloaded{
    [](std::monostate) {},
    [](auto& e) { e->sync(); },
  }, backend);

  if (not style)
    return;

  if (isDiscrete())
    style->setObjectType(PlotStyle::Discrete);
  else if (isContinuous())
    style->setObjectType(PlotStyle::Continuous);
  else style->setObjectType(PlotStyle::NonRepresentable);
}

}
