#include "zcmathobjectbb.h"
#include "information.h"

namespace zg {
namespace mathobj {
namespace shared {

ZcMathObjectBB::ZcMathObjectBB()
  : zcMathObj(mathWorld.new_object())
{
}

ZcMathObjectBB::~ZcMathObjectBB()
{
  mathWorld.erase(zcMathObj);
}

}
}
}
