#include "zcbase.h"
#include "information.h"

namespace zg {
namespace mathobj {

ZcBase::ZcBase(QObject *parent)
  : QObject(parent), zcMathObj(information.getMathWorld().new_object())
{
}

bool ZcBase::isValid() const
{
  return optError.getType() == OptError::VALID;
}

ZcBase::~ZcBase()
{
  information.getMathWorld().erase(zcMathObj);
}

}
}
