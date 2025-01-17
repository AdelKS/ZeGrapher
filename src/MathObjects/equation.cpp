#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : shared::StateBB(parent), shared::ZcMathObjectBB()
{}

void Equation::setEquation(QString eq)
{
  if (eq == equation)
    return;

  equation = std::move(eq);
  QString oldName = name;
  refresh();

  if ((not oldName.isEmpty() or not name.isEmpty()) and slot)
    information.mathObjectUpdated(*slot, oldName, name);
}

void Equation::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

void Equation::refresh()
{
  qDebug() << "[backend] zc: refreshing evaluation of equation: " << equation;

  zcMathObj = equation.toStdString();;
  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  if (state)
    state->update(zcMathObj.status());
}

}
}
