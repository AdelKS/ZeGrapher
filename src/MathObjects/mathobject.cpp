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

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
