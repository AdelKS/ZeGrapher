#include "zcmathobjectbb.h"
#include "MathObjects/mathworld.h"

namespace zg {
namespace mathobj {
namespace shared {

ZcMathObjectBB::ZcMathObjectBB()
  : zcMathObj(zc::mathWorld.new_object())
{
}

ZcMathObjectBB::~ZcMathObjectBB()
{
  zc::mathWorld.erase(zcMathObj);
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
