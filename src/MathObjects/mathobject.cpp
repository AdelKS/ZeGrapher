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

  PlotStyle::ObjectType objectType = PlotStyle::NonRepresentable;
  State new_state = std::visit(
    zc::utils::overloaded{
      [&](mathobj::Constant* v) {
        return v->setName(expr);
      },
      [&](mathobj::Expr* v) {
        return v->setExpression(expr);
      },
      [&](mathobj::Equation* v) {
        State state = v->setEquation(expr);
        if (style and state.getStatus() == State::VALID)
        {
          if (v->zcMathObj.holds(zc::ObjectType::DATA) or v->zcMathObj.holds(zc::ObjectType::SEQUENCE))
            objectType = PlotStyle::Discrete;
          else objectType = PlotStyle::Continuous;
        }
        return state;
      },
      [&](std::monostate) {
        return State();
      },
    },
    backend
  );

  if (style)
    style->setObjectType(objectType);

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

const zc::DynMathObject<zc_t>* MathObject::getZcObject() const
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

QString MathObject::getName() const
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

QStringList MathObject::directDependencies() const
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
