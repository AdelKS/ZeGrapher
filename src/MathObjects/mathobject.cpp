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
        if (const auto* zc_obj = e->getZcObject())
          return QString::fromStdString(std::string(zc_obj->get_name()));
        else return QString();
      },
      [](const Parametric*) {
        return QString();
      },
      [](std::monostate) { return QString{}; },
    },
    backend
  );
}

QStringList MathObject::directDependencies() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const ZcMathObject* e) {
        if (const auto* zc_obj = e->getZcObject())
          return QStringList{QString::fromStdString(std::string(zc_obj->get_name()))};
        else return QStringList();
      },
      [](const Parametric* p) {
        QStringList res;
        if (const auto* zc_obj = p->obj1->getZcObject())
          res << QString::fromStdString(std::string(zc_obj->get_name()));
        if (const auto* zc_obj = p->obj2->getZcObject())
          res << QString::fromStdString(std::string(zc_obj->get_name()));
        return res;
      },
      [](std::monostate) { return QStringList{}; },
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

void MathObject::refresh()
{
  std::visit(zc::utils::overloaded{
    [](std::monostate) {return State(); },
    [](auto* e) { return e->refresh(); },
  }, backend);
  updateMetadata();
}

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
