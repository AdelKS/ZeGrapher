/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#pragma once

#include "animatedconstant.h"

#include <QAbstractListModel>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/qmlsingleton.h"

#include <vector>
#include <chrono>

namespace zg {

using time_point = std::chrono::time_point<std::chrono::steady_clock>;


/// @brief singleton class that handles incrementing every animated variable altogether
///        before triggering a graph redraw.
class AnimationConductor: public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(bool animating READ isAnimating NOTIFY animatingChanged)

public:
  /// @brief QML uses the global instance instead of creating one
  static AnimationConductor* create(QQmlEngine*, QJSEngine*);

  explicit AnimationConductor(QObject *parent): QObject(parent) {}

  void track(zg::AnimatedConstant*);
  void untrack(zg::AnimatedConstant*);
  Q_INVOKABLE void animationStep();

  bool isAnimating() const {return animating; }

protected slots:
  void updateIsAnimating();

signals:
  void animatingChanged();

  /// @brief the animation made one step forward
  /// @note only is triggered when a constant is actually animating
  void tick();

protected:
  std::vector<zg::AnimatedConstant*> constants;

  /// @brief whether any constant is being animated
  bool animating = false;

  std::optional<time_point> last_step_timestamp;
};

/// @brief set by Globals
inline AnimationConductor* animationConductor = nullptr;

inline AnimationConductor* AnimationConductor::create(QQmlEngine*, QJSEngine*)
{
  return qmlSingleton(animationConductor);
}

} // namespace zg
