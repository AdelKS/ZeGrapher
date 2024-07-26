#include "MathObjects/mathobject.h"
#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{
  information.registerMathObject(this);
}

void MathObject::setBackend(mathobj::ZC* b)
{
  backend = b;
}

void MathObject::setBackend(mathobj::Expr* b)
{
  backend = b;
}

QStringList MathObject::handledMathObjects() const
{
  if (std::holds_alternative<zg::mathobj::Expr*>(backend))
    return QStringList(std::get<zg::mathobj::Expr*>(backend)->getImplicitName());
  else if (std::holds_alternative<zg::mathobj::ZC*>(backend))
  {
    QString name = std::get<zg::mathobj::ZC*>(backend)->getName();
    if (not name.isEmpty())
      return QStringList(name);
  }
  return QStringList();
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
