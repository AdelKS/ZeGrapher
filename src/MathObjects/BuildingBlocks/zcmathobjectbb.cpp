#include "zcmathobjectbb.h"
#include "globalvars.h"

namespace zg {
namespace mathobj {
namespace shared {

ZcMathObjectBB::ZcMathObjectBB()
  : zcMathObj(zg::mathWorld.backend.new_object())
{
}

ZcMathObjectBB::~ZcMathObjectBB()
{
  std::ignore = zg::mathWorld.backend.erase(zcMathObj);
}

bool ZcMathObjectBB::isContinuous() const
{
  if (zcMathObj)
    return not(zcMathObj.holds(zc::ObjectType::DATA) or zcMathObj.holds(zc::ObjectType::SEQUENCE));
  else return false;
}

bool ZcMathObjectBB::isDiscrete() const
{
  if (zcMathObj)
    return zcMathObj.holds(zc::ObjectType::DATA) or zcMathObj.holds(zc::ObjectType::SEQUENCE);
  else return false;
}

}
}
}
