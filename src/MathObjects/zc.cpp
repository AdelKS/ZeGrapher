#include "MathObjects/zc.h"
#include "information.h"

namespace zg {
namespace mathobj {

ZC::ZC(QObject *parent)
  : Base(parent)
{

}

void ZC::setEquation(QString eq)
{
  if (eq == equation)
    return;

  equation = std::move(eq);
  QString oldName = name;
  refresh();

  if (not oldName.isEmpty() or not name.isEmpty())
    information.mathObjectUpdated(oldName, name);
}

void ZC::refresh(bool canChangeType)
{
  std::string std_eq = equation.toStdString();
  switch (type)
  {
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
    Q_ASSERT(false);
    break;
  }

  if (canChangeType and not zcMathObj.has_value())
  {
    // if the current type doesn't work, try AUTO
    zcMathObj = std_eq;
    if (zcMathObj.has_value())
    {
      // if it works, update type and notify about it
      if (zcMathObj.holds<zc::Function<zc_t>>() and type != Type::FUNCTION)
      {
        type = Type::FUNCTION;
        emit typeChanged(type);
      }
      else if (zcMathObj.holds<zc::Sequence<zc_t>>() and type != Type::SEQUENCE)
      {
        type = Type::SEQUENCE;
        emit typeChanged(type);
      }
      else if (zcMathObj.holds<zc::GlobalConstant>() and type != Type::CONSTANT)
      {
        type = Type::CONSTANT;
        emit typeChanged(type);
      }
      else
      {
        qDebug() << "[backend] setEquation: case not handled";
        Q_ASSERT(false);
      }
    }
  }

  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  if (state)
    state->update(zcMathObj);
}

void ZC::setType(Type type)
{
  if (this->type == type)
    return;

  this->type = type;
  refresh(false);

}

}
}
