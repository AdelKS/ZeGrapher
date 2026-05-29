#include "MathObjects/constant.h"
#include "globalvars.h"
namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): Stateful(parent)
{
  zcMathObj = std::nan("");
}

Constant::~Constant()
{
  mathWorld.unsetSchrodingerConstant(this);
}

State Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
    return sync();

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  emit updated();

  return sync();
}

void Constant::setDeadAndAlive(bool s)
{
  if (s == deadAndAlive)
    return;

  deadAndAlive = s;

  /// @brief give a NaN value so Exprs that depend on it get a warning color
  if (deadAndAlive)
  {
    mathWorld.setSchrodingerConstant(this);
    zcMathObj = std::nan("");
    value = std::nan("");
    emit valueChanged();
  }
  else
  {
    mathWorld.unsetSchrodingerConstant(this);
    if (not std::isnan(from) and not std::isnan(to))
    {
      const double val = (from + to) / 2.;
      zcMathObj = val;
      value = val;
      emit valueChanged();
    }
  }

  emit deadAndAliveChanged();
  emit updated();
}

void Constant::setLoopType(LoopType t)
{
  if (t == loopType)
    return;

  loopType = t;

  emit loopTypeChanged();
}


void Constant::setFrom(double v)
{
  if (v == from) return;
  from = v;

  emit fromChanged();
  emit updated();
}

void Constant::setTo(double v)
{
  if (v == to) return;
  to = v;

  emit toChanged();
  emit updated();
}

void Constant::setSteps(int v)
{
  if (v == steps or v <= 0) return;
  steps = v;

  emit stepsChanged();
  emit updated();
}

void Constant::constrainValue()
{
  if (std::isnan(from) or std::isnan(to))
    return;

  const double min = std::min(from, to);
  const double max = std::max(from, to);

  if (value < min)
    set_value(min);

  else if (max < value)
    set_value(max);
}

void Constant::setDuration(double d)
{
  if (seconds(d) == duration) return;

  duration = seconds(d);
}

bool Constant::isValid()
{
  sync();
  return getState().isValid();
}

State Constant::sync() {
  setState(State::from(zcMathObj.name_status()));

  return getState();
};

void Constant::set_value(double val)
{
  if (value == val)
    return;

  zcMathObj = val;
  value = val;

  from = lowerMul * val;
  to = upperMul * val;

  emit fromChanged();
  emit toChanged();
  emit valueChanged();
  emit updated();
}

void Constant::setLerpValue(double a)
{
  const double newVal = std::lerp(from, to, a);
  value = newVal;
  zcMathObj = newVal;

  emit valueChanged();
  emit updated();
}

}
}
