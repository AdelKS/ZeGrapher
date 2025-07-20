#include "MathObjects/zcmathobject.h"

namespace zg {

ZcMathObject::ZcMathObject(QObject *parent) : QObject(parent)
{}

void ZcMathObject::setBackend(mathobj::Equation* b)
{
  Q_ASSERT(b);
  backend = b;
  if (slot)
    b->setSlot(*slot);
}

void ZcMathObject::setBackend(mathobj::Expr* b)
{
  Q_ASSERT(b);
  backend = b;
  if (slot)
    b->setSlot(*slot);
}

void ZcMathObject::setBackend(mathobj::Constant* b)
{
  Q_ASSERT(b);
  backend = b;
  if (slot)
    b->setSlot(*slot);
}

void ZcMathObject::setBackend(mathobj::NamedRef* n)
{
  Q_ASSERT(n);
  backend = n;
  if (slot)
    n->setSlot(*slot);
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
      [&](mathobj::NamedRef* n) {
        return n->setName(expr);
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
  using Ret = const zc::DynMathObject<zc_t>*;
  return std::visit(
    zc::utils::overloaded{
      [&](const auto* c) -> Ret {
        return &c->zcMathObj;
      },
      [](const mathobj::NamedRef* n) -> Ret {
        return n->getZcObject();
      },
      [](std::monostate) -> Ret {
        return nullptr;
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

State ZcMathObject::sync()
{
  State old_state = state;

  state = std::visit(zc::utils::overloaded{
    [](std::monostate) {return State(); },
    [](mathobj::Expr* e) {
      e->updateValue();
      return e->getState();
    },
    [](auto* b) { return b->getState(); },
  }, backend);

  if (old_state != state)
  {
    if (highlighter)
      highlighter->rehighlight();

    emit stateChanged();
  }

  return state;
}

}
