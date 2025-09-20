#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent, Type type) : QObject(parent)
{
  setType(type);
}

void MathObject::setType(Type t)
{
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
    case PARAMETRIC:
      backend = new mathobj::Parametric(this);
    break;
  }

  std::visit(
    zc::utils::overloaded{
      [this]<typename T>(T* n) {
        connect(n, &T::updated, this, [this]{ emit updated(this); });
      },
      [](std::monostate) {},
    },
    backend
  );

  emit typeChanged();
}

MathObject::Type MathObject::getType() const
{
  return std::visit(
    zc::utils::overloaded{
      [&](std::monostate) {
        return MONOSTATE;
      },
      [](const mathobj::Equation*) {
        return EQUATION;
      },
      [](const mathobj::Expr*) {
        return EXPR;
      },
      [](const mathobj::Constant*) {
        return CONSTANT;
      },
      [](const mathobj::NamedRef*) {
        return NAMEDREF;
      },
      [](const mathobj::Data*) {
        return DATA;
      },
      [](const mathobj::Parametric*) {
        return PARAMETRIC;
      },
    },
    backend
  );
}

mathobj::Equation* MathObject::getEquation()
{
  return getBackend<mathobj::Equation>();
}

mathobj::Expr* MathObject::getExpr()
{
  return getBackend<mathobj::Expr>();
}

mathobj::Constant* MathObject::getConstant()
{
  return getBackend<mathobj::Constant>();
}

mathobj::NamedRef* MathObject::getNamedRef()
{
  return getBackend<mathobj::NamedRef>();
}

mathobj::Data* MathObject::getData()
{
  return getBackend<mathobj::Data>();
}


mathobj::Parametric* MathObject::getParametric()
{
  return getBackend<mathobj::Parametric>();
}

MathObject::EvalHandle MathObject::getZcObject() const
{
  using Ret = MathObject::EvalHandle;
  return std::visit(
    zc::utils::overloaded{
      [&](const auto* c) -> Ret {
        return &c->zcMathObj;
      },
      [](const mathobj::Parametric* p) -> Ret {
        return std::make_pair(p->obj1->getZcObject(), p->obj2->getZcObject());
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

bool MathObject::isContinuous() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* c) {
        return c->isContinuous();
      },
      [](const mathobj::Parametric* p) {
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
      [](const auto* c) {
        return c->isDiscrete();
      },
      [](const mathobj::Parametric* p) {
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
      [](const auto* e) {
        return e->getState().getStatus() == State::VALID;
      },
      [](const mathobj::Parametric* p) {
        return p->obj1->isValid() and p->obj2->isValid();
      },
      [](std::monostate) { return false; },
    },
    backend
  );
}

size_t MathObject::getRevision() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* e) {
        return e->zcMathObj.get_revision();
      },
      [](const mathobj::NamedRef* r) {
        if (const auto* o = r->getZcObject())
          return o->get_revision();
        else return 0ul;
      },
      [](const mathobj::Parametric* p) {
        size_t rev1 = 0, rev2 = 0;
        if (const auto* o1 = p->obj1->getZcObject())
          rev1 = o1->get_revision();
        if (const auto* o2 = p->obj1->getZcObject())
          rev2 = o2->get_revision();
        return rev1 + rev2;
      },
      [](std::monostate) { return 0ul; },
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
      [](const mathobj::Parametric*) {
        return QString();
      },
      [](std::monostate) { return QString{}; },
    },
    backend
  );
}

State MathObject::sync()
{
  State oldState = state;

  state = std::visit(zc::utils::overloaded{
    [](std::monostate) { return State(); },
    [](auto* e) { return e->sync(); },
  }, backend);

  if (oldState != state)
    emit stateChanged(this);

  if (not style)
    return state;

  if (isDiscrete())
    style->setObjectType(PlotStyle::Discrete);
  else if (isContinuous())
    style->setObjectType(PlotStyle::Continuous);
  else style->setObjectType(PlotStyle::NonRepresentable);

  return state;
}

}
