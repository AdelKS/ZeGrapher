#include "animatedconstant.h"
#include "globalvars.h"

namespace zg {

bool AnimatedConstant::isDeadAndAlive() const
{
  return deadAndAlive;
}

void AnimatedConstant::setDeadAndAlive(bool s)
{
  if (s == deadAndAlive)
    return;

  deadAndAlive = s;

  if (s)
    setPlaying(false);

  emit deadAndAliveChanged();
}

void AnimatedConstant::setBackend(mathobj::Constant* b)
{
  backend = b;
  connect(b, &mathobj::Constant::valueChanged, this, &AnimatedConstant::valueChanged);
}

void AnimatedConstant::animationStep(seconds T)
{
  // in case the step took too long
  phase = (phase * duration.count() + T.count() ) / duration.count();

  switch (loopType)
  {
    case REPEAT:    sliderPos = std::fmod(phase, 1.0); break;
    case ONESHOT:   sliderPos = phase < 1.0 ? phase : 1.0; break;
    case PING_PONG: sliderPos = phase <= 1. ? phase : 2 - phase; break;
  }

  if (loopType == ONESHOT and phase >= 1.0)
  {
    phase = 1.0;
    setPlaying(false);
  }
  else phase = std::fmod(phase, loopType == PING_PONG ? 2.0 : 1.0);

  backend->value = (1. - sliderPos) * from + sliderPos * to;
  backend->zcMathObj = backend->value;

  emit valueChanged();
  emit sliderPosChanged();
}

void AnimatedConstant::setPlaying(bool p)
{
  if (p == playing)
    return;

  if (p and deadAndAlive)
    return;

  playing = p;

  emit playingChanged();
}

void AnimatedConstant::setDuration(double d)
{
  if (std::isnan(d))
    setPlaying(false);

  duration = seconds(d);
}

void AnimatedConstant::setValue(double v)
{
  if (backend->value == v)
    return;

  if (std::isnan(v))
    setPlaying(false);

  sliderPos = 0.5;
  phase = 0.5;

  backend->set_value(v);

  from = lowerMul * v;
  to = upperMul * v;

  emit valueChanged();
  emit fromChanged();
  emit toChanged();
  emit sliderPosChanged();
}

void AnimatedConstant::setFrom(double v)
{
  if (from == v)
    return;

  if (std::isnan(v))
    setPlaying(false);

  from = v;

  constrainValue();
  recomputeSliderPos();

  emit fromChanged();
}

void AnimatedConstant::setTo(double v)
{
  if (to == v)
    return;

  if (std::isnan(v))
    setPlaying(false);

  to = v;

  constrainValue();
  recomputeSliderPos();

  emit toChanged();
}

void AnimatedConstant::setSteps(size_t v)
{
  if (steps == v)
    return;

  steps = v;

  emit stepsChanged();
}

void AnimatedConstant::setSliderPos(double v)
{
  if (sliderPos == v)
    return;

  if (deadAndAlive)
    return;

  assert(0. <= v);
  assert(v <= 1.0);

  sliderPos = v;
  phase = sliderPos;

  backend->set_value(std::lerp(from, to, sliderPos));

  emit sliderPosChanged();
}

void AnimatedConstant::constrainValue()
{
  double value = backend->get_value();

  if (std::isnan(from) or std::isnan(to))
    return;

  const bool inverted = from > to;
  const double min = std::min(from, to);
  const double max = std::max(from, to);

  if (value < min)
  {
    backend->set_value(min);
    sliderPos = inverted ? 1.0 : 0.;
    phase = sliderPos;

    emit sliderPosChanged();
  }
  else if (max < value)
  {
    backend->set_value(max);
    sliderPos = inverted ? 0. : 1.;
    phase = sliderPos;

    emit sliderPosChanged();
  }
}

void AnimatedConstant::setLoopType(LoopType t)
{
  if (t == loopType)
    return;

  loopType = t;

  // handle the case PING_PONG -> something else properly
  phase = sliderPos;
}

void AnimatedConstant::recomputeSliderPos()
{
  const double value = backend->value;

  if (std::isnan(from) or std::isnan(to))
    return;

  const bool inverted = from > to;
  const double min = std::min(from, to);
  const double max = std::max(from, to);
  const double amplitude = max - min;

  if (amplitude == 0.0)
  {
    sliderPos = 0.5;
  }
  else
  {
    if (inverted)
      sliderPos = (max - value) / amplitude;
    else
      sliderPos = (value - min) / amplitude;
  }

  phase = sliderPos;

  emit sliderPosChanged();
}

}
