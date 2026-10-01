#include "zcmathobjectbb.h"
#include "information.h"

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
  return zcMathObj and not holdsDiscrete(zcMathObj);
}

bool ZcMathObjectBB::isDiscrete() const
{
  return zcMathObj and holdsDiscrete(zcMathObj);
}

}
}
}
