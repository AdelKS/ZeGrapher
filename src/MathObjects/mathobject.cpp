#include "MathObjects/mathobject.h"

namespace zg {

MathObject::MathObject(QObject *parent) : QObject(parent)
{

}

void MathObject::setBackend(mathobj::ZC* backend)
{
  zcBackend = backend;
}

}
