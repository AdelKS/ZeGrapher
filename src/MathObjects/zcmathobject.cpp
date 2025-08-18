#include "MathObjects/zcmathobject.h"
#include "information.h"

namespace zg {

ZcMathObject::ZcMathObject(QObject *parent) : QObject(parent)
{}

void ZcMathObject::setBackend(ZcMathObject::BackendType t)
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
    case EQUATION:
      backend = new mathobj::Equation(this);
    break;
    case EXPR:
      backend = new mathobj::Expr(this);
    break;
    case CONSTANT:
      backend = new mathobj::Constant(this);
    break;
    case NAMEDREF:
      backend = new mathobj::NamedRef(this);
    break;
    case DATA:
      backend = new mathobj::Data(this);
    break;
  }
}

mathobj::Equation* ZcMathObject::getEquation()
{
  return getBackend<mathobj::Equation>();
}

mathobj::Expr* ZcMathObject::getExpr()
{
  return getBackend<mathobj::Expr>();
}

mathobj::Constant* ZcMathObject::getConstant()
{
  return getBackend<mathobj::Constant>();
}

mathobj::NamedRef* ZcMathObject::getNamedRef()
{
  return getBackend<mathobj::NamedRef>();
}

mathobj::Data* ZcMathObject::getData()
{
  return getBackend<mathobj::Data>();
}

bool ZcMathObject::isValid() const
{
  return getState().getStatus() == State::VALID;
}

State ZcMathObject::setExpression(QString expr)
{
  State old_sate = state;

  state = std::visit(
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
      [&](mathobj::Data* n) {
        return n->setName(expr);
      },
      [&](std::monostate) {
        return State();
      },
    },
    backend
  );

  information.mathObjectUpdated();

  if (old_sate != state)
    emit stateChanged();

  return state;
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
  return state;
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
    [](auto* b) { return b->sync(); },
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
