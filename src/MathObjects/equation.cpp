#include "MathObjects/equation.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : QObject(parent), shared::ZcMathObjectBB()
{}

State Equation::setEquation(QString eq)
{
  if (eq == equation)
    return state;

  equation = std::move(eq);
  QString oldName = name;
  refresh();

  if ((not oldName.isEmpty() or not name.isEmpty()) and slot)
    information.mathObjectUpdated(*slot, oldName, name);

  return state;
}

void Equation::setSlot(size_t slot)
{
  this->slot = slot;
  static_cast<shared::ZcMathObjectBB&>(*this).slot = slot;
}

QString Equation::getName() const { return name; }
State Equation::getState() const { return state; };
void Equation::setState(State newState) { state = newState; }

State Equation::refresh()
{
  qDebug() << "[backend] zc: refreshing evaluation of equation: " << equation;

  zcMathObj = equation.toStdString();;
  name = QString::fromStdString(std::string(zcMathObj.get_name()));

  state.update(zcMathObj.status());
  return state;
}

}
}
