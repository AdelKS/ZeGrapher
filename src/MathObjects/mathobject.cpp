#include "MathObjects/mathobject.h"
#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{
  information.registerMathObject(this);
}

void MathObject::setBackend(mathobj::Equation* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

void MathObject::setBackend(mathobj::Expr* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

void MathObject::setBackend(mathobj::Constant* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
}

State MathObject::setExpression(QString expr)
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
      [](std::monostate) {
        return State();
      },
    },
    backend
  );

  if (old_sate != new_state)
    emit stateChanged();

  return new_state;
}

State MathObject::getState() const
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

void MathObject::setState(State newState)
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

void MathObject::setSlot(size_t slot)
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

QStringList MathObject::handledMathObjects() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const zg::mathobj::Expr* expr) -> QStringList {
        return {expr->getImplicitName()};
      },
      [](const zg::mathobj::Equation* eq) -> QStringList {
        if (QString name = eq->getName(); not name.isEmpty())
          return {name};
        else return {};
      },
      [](const zg::mathobj::Constant* cst) -> QStringList {
        if (QString name = cst->getName(); not name.isEmpty())
          return {name};
        else return {};
      },
      [](std::monostate) -> QStringList { return {}; },
    },
    backend
  );
}

State MathObject::refresh()
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

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
