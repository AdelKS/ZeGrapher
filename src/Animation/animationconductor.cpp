/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
  if (not lastStepTimestamp)
  {
    lastStepTimestamp = std::chrono::steady_clock::now();
    return;
  }
  else
  {
    auto now = std::chrono::steady_clock::now();

    for (auto* constant: constants)
      if (constant->isPlaying())
        constant->animationStep(now - *lastStepTimestamp);

    lastStepTimestamp = now;
  }
}

void AnimationConductor::updateIsAnimating()
{
  bool oldAnimating = animating;
  animating = std::ranges::any_of(constants, [](const AnimatedConstant* c){ return c->isPlaying(); });

  if (oldAnimating != animating)
  {
    lastStepTimestamp.reset();
    emit animatingChanged();
  }
}

}
