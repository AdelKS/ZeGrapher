#include "opterror.h"
#include "zc.h"

namespace  zg {

OptError::Type OptError::getType() const
{
  if (value)
  {
    if (value->type == zc::Error::EMPTY_EXPRESSION)
      return NEUTRAL;
    else return INVALID;
  }
  else return VALID;
}

QString OptError::getMessage() const
{
  if (value)
    return zcErrorToStr(*value);
  else return "";
}

}