#include "MathObjects/zc.h"
#include "information.h"

namespace zg {
namespace mathobj {

ZC::ZC(QObject *parent)
  : ZcBase(parent)
{

}

void ZC::setEquation(QString eq)
{
  equation = std::move(eq);
  QString oldName = name;
  refresh();

  if (not oldName.isEmpty() or not name.isEmpty())
    information.mathObjectUpdated(oldName, name);
}

void ZC::refresh()
{
  std::string std_eq = equation.toStdString();
  switch (type)
  {
  case Type::AUTO:
    zcMathObj = std_eq;
    break;
  case Type::FUNCTION:
    zcMathObj = zc::As<zc::Function<zc_t>>{std_eq};
    break;
  case Type::CONSTANT:
    zcMathObj = zc::As<zc::GlobalConstant>{std_eq};
    break;
  case Type::SEQUENCE:
    zcMathObj = zc::As<zc::Sequence<zc_t>>{std_eq};
    break;
  default:
    qDebug() << "[backend] setEquation: case not handled";
    break;
  }

  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  OptError oldOptError = optError;
  if (not zcMathObj.has_value())
    optError.value = zcMathObj.error();
  else optError.value.reset();

  if (oldOptError != optError)
    emit optErrorChanged();
}

void ZC::setType(Type type)
{
  this->type = type;
  refresh();
}

}
}
