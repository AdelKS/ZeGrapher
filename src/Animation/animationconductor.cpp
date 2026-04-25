#include "animationconductor.h"

namespace zg {

void AnimationConductor::track(AnimatedConstant* c)
{
  constants.push_back(c);
  updateIsAnimating();
  connect(c, &AnimatedConstant::playingChanged, this, &AnimationConductor::updateIsAnimating);
}

void AnimationConductor::untrack(zg::AnimatedConstant* c)
{
  if (auto it = std::ranges::find(constants, c); it != constants.end())
  {
    constants.erase(it);
    updateIsAnimating();
    disconnect(c, &AnimatedConstant::playingChanged, this, &AnimationConductor::updateIsAnimating);
  }
}

void AnimationConductor::animationStep()
{
  if (not last_step_timestamp)
  {
    last_step_timestamp = std::chrono::high_resolution_clock::now();
    return;
  }
  else
  {
    auto now = std::chrono::high_resolution_clock::now();

    for (auto* constant: constants)
      if (constant->isPlaying())
        constant->animationStep(now - *last_step_timestamp);

    last_step_timestamp = now;

    emit tick();
  }
}

void AnimationConductor::updateIsAnimating()
{
  bool oldAnimating = animating;
  animating = std::ranges::any_of(constants, [](const AnimatedConstant* c){ return c->isPlaying(); });

  if (oldAnimating != animating)
  {
    last_step_timestamp.reset();
    emit animatingChanged();
  }
}

}
