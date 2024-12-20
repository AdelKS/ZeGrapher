#include "base.h"
#include "information.h"

namespace zg {
namespace mathobj {

Base::Base(QObject *parent)
  : QObject(parent), zcMathObj(information.getMathWorld().new_object())
{
}

bool Base::isValid() const
{
  if (state)
    return state->getStatus() == State::VALID;
  else return false;
}

Base::~Base()
{
  information.getMathWorld().erase(zcMathObj);
}

}
}
