#include "MathObjects/zcmathobject.h"

namespace zg {

ZcMathObject::ZcMathObject(QObject *parent) : QObject(parent)
{}

void ZcMathObject::setBackend(mathobj::Equation* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

void ZcMathObject::setBackend(mathobj::Expr* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

void ZcMathObject::setBackend(mathobj::Constant* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

bool ZcMathObject::isValid() const
{
  return getState().getStatus() == State::VALID;
}

State ZcMathObject::setExpression(QString expr)
{
  State old_sate = getState();

  State new_state = std::visit(
    zc::utils::overloaded{
      [&](mathobj::Constant* v) {
        return v->setName(expr);
      },
      [&](mathobj::Expr* v) {
        return v->setExpression(expr);
      },
      [&](mathobj::Equation* v) {
        return v->setEquation(expr);
      },
      [&](std::monostate) {
        return State();
      },
    },
    backend
  );

  if (old_sate != new_state)
    emit stateChanged();

  return new_state;
}

bool ZcMathObject::isContinuous() const
{
  if (const auto* v = getZcObject())
    return not (v->holds(zc::ObjectType::DATA) or v->holds(zc::ObjectType::SEQUENCE));
  else return false;
}

bool ZcMathObject::isDiscrete() const
{
  if (const auto* v = getZcObject())
    return v->holds(zc::ObjectType::DATA) or v->holds(zc::ObjectType::SEQUENCE);
  else return false;
}

State ZcMathObject::getState() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](const auto* v) {
        return v->getState();
      },
      [](std::monostate) {
        return State();
      },
    },
    backend
  );
}

const zc::DynMathObject<zc_t>* ZcMathObject::getZcObject() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](const auto* c) {
        return &c->zcMathObj;
      },
      [](std::monostate) -> zc::DynMathObject<zc_t>*{
        return nullptr;
      },
    },
    backend
  );

}

void ZcMathObject::setState(State newState)
{
  std::visit(
    zc::utils::overloaded{
      [&](auto* v) {
        return v->setState(newState);
      },
      [](std::monostate) {
      },
    },
    backend
  );
}

void ZcMathObject::setSlot(size_t slot)
{
  this->slot = slot;
  std::visit(
    zc::utils::overloaded{
      [slot](auto* v) {
        v->setSlot(slot);
      },
      [](std::monostate) {},
    },
    backend
  );
}

QString ZcMathObject::getName() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* e) {
        return e->getName();
      },
      [](std::monostate) { return QString{}; },
    },
    backend
  );
}

QStringList ZcMathObject::directDependencies() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* e) {
        return QStringList{e->getName()};
      },
      [](std::monostate) { return QStringList{}; },
    },
    backend
  );
}

State ZcMathObject::refresh()
{
  State old_sate = getState();

  State new_state = std::visit(zc::utils::overloaded{
    [](std::monostate) {return State(); },
    [](auto* b) { return b->refresh(); },
  }, backend);

  if (old_sate != new_state)
    emit stateChanged();

  if (highlighter)
    highlighter->rehighlight();

  return new_state;
}

}
