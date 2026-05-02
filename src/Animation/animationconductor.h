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

#include <vector>
#include <chrono>

namespace zg {

using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;


/// @brief singleton class that handles incrementing every animated variable altogether
///        before triggering a graph redraw.
class AnimationConductor: public QObject
{
  Q_OBJECT
  QML_ANONYMOUS

  Q_PROPERTY(bool animating READ isAnimating NOTIFY animatingChanged)
  Q_PROPERTY(zg::AnimatedConstant* schrodingerConstant WRITE setSchrodingerConstant MEMBER schrodingerConstant NOTIFY schrodingerConstantChanged)

public:
  AnimationConductor(QObject *parent = nullptr): QObject(parent) {}

  Q_INVOKABLE void track(zg::AnimatedConstant*);
  Q_INVOKABLE void untrack(zg::AnimatedConstant*);
  Q_INVOKABLE void animationStep();

  Q_INVOKABLE bool isAnimating() const {return animating; }

  Q_INVOKABLE void setSchrodingerConstant(zg::AnimatedConstant* c);
  Q_INVOKABLE void unsetSchrodingerConstant(zg::AnimatedConstant* c);
  zg::AnimatedConstant* getSchrodingerConstant() { return schrodingerConstant; }

protected slots:
  void updateIsAnimating();

signals:
  void animatingChanged();

  /// @brief the animation made one step forward
  /// @note only is triggered when a constant is actually animating
  void tick();

  void schrodingerConstantChanged();

protected:
  std::vector<zg::AnimatedConstant*> constants;

  /// @brief whether any constant is being animated
  bool animating = false;

  zg::AnimatedConstant* schrodingerConstant = nullptr;

  std::optional<time_point> last_step_timestamp;
};

} // namespace zg
