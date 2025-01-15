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

void MathObject::setBackend(mathobj::Parametric* b)
{
  backend = b;
  if (slot)
    setSlot(*slot);
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
      [](const zg::mathobj::Parametric* par) -> QStringList {
        QStringList res;
        if (not par->getFirstName().isEmpty())
          return res << par->getFirstName();
        if (not par->getSecondName().isEmpty())
          return res << par->getSecondName();
        return res;
      },
      [](std::monostate) -> QStringList { return {}; },
    },
    backend
  );
}

void MathObject::refresh()
{
  std::visit(zc::utils::overloaded{
    [](std::monostate) {},
    [](auto* b) { b->refresh(); },
  }, backend);
}

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
