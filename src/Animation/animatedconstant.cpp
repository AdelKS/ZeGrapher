#include "animatedconstant.h"

namespace zg {

void AnimatedConstant::setBackend(mathobj::Constant* b)
{
  backend = b;
  connect(b, &mathobj::Constant::fromChanged, this, &AnimatedConstant::backendUpdate);
  connect(b, &mathobj::Constant::toChanged, this, &AnimatedConstant::backendUpdate);
  connect(b, &mathobj::Constant::valueChanged, this, &AnimatedConstant::backendUpdate);
  connect(b, &mathobj::Constant::deadAndAliveChanged, this, &AnimatedConstant::backendUpdate);
}

void AnimatedConstant::animationStep(seconds T)
{
  // in case the step took too long
  phase = (phase * backend->duration.count() + T.count() ) / backend->duration.count();

  if (backend->loopType == mathobj::Constant::ONESHOT and (phase > 1.0 or phase < 0.))
  {
    phase = std::clamp(phase, 0.0, 1.0);
    setPlaying(false);
  }
  else phase = std::fmod(phase, backend->loopType == mathobj::Constant::PING_PONG ? 2.0 : 1.0);

  assert(phase >= 0.);

  updateSliderPosFromPhase();
}

void AnimatedConstant::setPlaying(bool p)
{
  if (p == playing)
    return;

  if (p and backend->deadAndAlive)
    return;

  playing = p;

  emit playingChanged();
}

void AnimatedConstant::setSliderPos(double v)
{
  if (sliderPos == v)
    return;

  if (backend->deadAndAlive)
    return;

  assert(0. <= v);
  assert(v <= 1.0);

  sliderPos = v;
  phase = sliderPos;

  ignoreSliderPosRecompute = true;
  backend->setLerpValue(sliderPos);
  ignoreSliderPosRecompute = false;

  emit sliderPosChanged();
}

void AnimatedConstant::backendUpdate()
{
  if (std::isnan(backend->from) or std::isnan(backend->to) or backend->deadAndAlive)
    setPlaying(false);

  updateSliderPosFromRange();
}

void AnimatedConstant::updateSliderPosFromPhase()
{
  const double oldPhase = phase;

  switch (backend->loopType)
  {
    case mathobj::Constant::REPEAT:
    case mathobj::Constant::ONESHOT:
      setSliderPos(phase);
      break;
    case mathobj::Constant::PING_PONG:
      setSliderPos(phase <= 1. ? phase : 2 - phase);
      break;
  }

  phase = oldPhase;
}

void AnimatedConstant::updateSliderPosFromRange()
{
  if (ignoreSliderPosRecompute)
    return;

  const double value = backend->value;

  if (std::isnan(backend->from) or std::isnan(backend->to))
  {
    sliderPos = 0.5;
  }
  else
  {
    const bool inverted = backend->from > backend->to;
    const double min = std::min(backend->from, backend->to);
    const double max = std::max(backend->from, backend->to);
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
  }

  phase = phase >= 1.0 ? 2. - sliderPos : sliderPos;

  emit sliderPosChanged();
}

}
