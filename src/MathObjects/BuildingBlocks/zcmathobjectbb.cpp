#include "zcmathobjectbb.h"
#include "information.h"

namespace zg {
namespace mathobj {
namespace shared {

ZcMathObjectBB::ZcMathObjectBB()
  : zcMathObj(information.getMathWorld().new_object())
{
}

ZcMathObjectBB::~ZcMathObjectBB()
{
  information.getMathWorld().erase(zcMathObj);
}

}
}
}
