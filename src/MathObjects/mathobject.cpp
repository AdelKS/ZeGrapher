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

const zc::DynMathObject<zc_t>* MathObject::getZcObject() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](const ZcMathObject* c) {
        return c->getZcObject();
      },
      [](std::monostate) -> const zc::DynMathObject<zc_t>* {
        return nullptr;
      },
    },
    backend
  );
}

bool MathObject::isContinuous() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](const ZcMathObject* c) {
        return c->isContinuous();
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
    [](auto* b) { return b->refresh(); },
  }, backend);
  updateMetadata();
}

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
