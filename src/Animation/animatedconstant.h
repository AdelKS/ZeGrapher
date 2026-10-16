#pragma once

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

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include <chrono>

namespace zg {

using seconds = std::chrono::duration<double>;

namespace mathobj {
  struct Constant;
}

/// @brief a constant's animation, which sweeps its value from 'from' to 'to'
/// @note the constant owns it, so it keeps playing while the constant's card
///       is scrolled out of the list
class AnimatedConstant: public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("the animation of a Constant, which creates it")

  Q_PROPERTY(bool playing WRITE setPlaying MEMBER playing NOTIFY playingChanged)
  Q_PROPERTY(double sliderPos WRITE setSliderPos MEMBER sliderPos NOTIFY sliderPosChanged)

public:
  explicit AnimatedConstant(mathobj::Constant* backend);
  ~AnimatedConstant();

  bool isPlaying() const {return playing;}

public slots:
  /// @brief make step, knowing that 'T' time has passed since the last step
  void animationStep(seconds T);

  void setSliderPos(double);
  void setPlaying(bool);

signals:
  void sliderPosChanged();
  void playingChanged();

protected slots:
  /// @brief ensure value is between from and to
  void backendUpdate();

protected:
  void updateSliderPosFromRange();
  void updateSliderPosFromPhase();

  mathobj::Constant* backend;
  bool playing = false;
  double sliderPos = 0.5;


  /// @brief it's sliderPos but in [0, 2] instead of [0, 1] interval
  ///        so it accounts for the ping-pong dynamic
  double phase = 0.5;

  bool ignoreSliderPosRecompute = false;

};

}
