#include "MathObjects/mathobject.h"
#include "information.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{
  information.registerMathObject(this);
}

void MathObject::setBackend(mathobj::ZC* backend)
{
  zcBackend = backend;
}

MathObject::~MathObject()
{
  information.deregisterMathObject(this);
}

}
