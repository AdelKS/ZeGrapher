#include "MathObjects/constant.h"
#include "Utils/yaml.h"
#include "information.h"

#include <QSignalBlocker>

namespace zg {
namespace mathobj {

Constant::Constant(QObject *parent): QObject(parent), animation(new AnimatedConstant(this))
{
  zcMathObj = std::nan("");
}

Constant::~Constant()
{
  // setSchrodingerConstant() updates the constant it unsets
  const QSignalBlocker blocker(this);
  if (isDeadAndAlive())
    mathWorld->setSchrodingerConstant(nullptr);
}

void Constant::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

void Constant::setName(QString new_input_name)
{
  if (input_name == new_input_name)
  {
    sync();
    return;
  }

  input_name = new_input_name;
  zcMathObj.set_name(input_name.toStdString());

  emit nameChanged();
  emit updated();

  sync();
}

bool Constant::isDeadAndAlive() const
{
  return mathWorld->getSchrodingerConstant() == this;
}

void Constant::setDeadAndAlive(bool s)
{
  if (s)
    mathWorld->setSchrodingerConstant(this);
  else if (isDeadAndAlive())
    mathWorld->setSchrodingerConstant(nullptr);
}

void Constant::schrodingerSwitched()
{
  // a NaN value gives the expressions that use the constant a warning color
  if (isDeadAndAlive())
  {
    zcMathObj = std::nan("");
    value = std::nan("");
  }
  else if (not std::isnan(from) and not std::isnan(to))
  {
    value = (from + to) / 2.;
    zcMathObj = value;
  }

  emit valueChanged();
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

void Constant::setDuration(double d)
{
  if (seconds(d) == duration) return;

  duration = seconds(d);

  emit durationChanged();
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

Constant::POD Constant::exportPod() const
{
  // set_value() puts the edges at lowerMul and upperMul times the value
  auto get_opt_edge = [this](double edge, double multiplier) -> std::optional<double> {
    if (std::isnan(edge) or edge == multiplier * value)
      return {};
    return edge;
  };

  return POD {
    .name = yml::not_default(getName()),
    .value = yml::not_nan(value),
    .from = get_opt_edge(from, lowerMul),
    .to = get_opt_edge(to, upperMul),
    .steps = yml::not_default(steps, defaultSteps),
    .loop_type = yml::not_default(loopType, defaultLoopType),
    .dead_and_alive = yml::not_default(isDeadAndAlive(), false),
    .duration = yml::not_default(duration.count(), defaultDuration.count())
  };
}

void Constant::importPod(Constant::POD p)
{
  if (p.name)
    setName(QString::fromStdString(*p.name));

  if (p.value)
    set_value(*p.value);

  if (p.from)
    setFrom(*p.from);

  if (p.to)
    setTo(*p.to);

  if (p.steps)
    setSteps(yml::clamped(*p.steps, 1, maxSteps));

  if (p.loop_type)
    setLoopType(*p.loop_type);

  if (p.dead_and_alive)
    setDeadAndAlive(*p.dead_and_alive);

  if (p.duration)
    setDuration(yml::clamped(*p.duration, minDuration, maxDuration));
}

}
}
