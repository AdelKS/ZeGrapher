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
  QString name = QString::fromStdString(std::string(zcMathObj.get_name()));

  information.getMathWorld().erase(zcMathObj);
  if (not name.isEmpty())
    information.mathObjectUpdated(name, name);
}

}
}
}
