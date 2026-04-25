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

#include "MathObjects/constant.h"

#include <cmath>
#include <chrono>

namespace zg {

using seconds = std::chrono::duration<double>;

/// @brief Wrapper around zg::mathobj::Constant with animation knobs added
class AnimatedConstant: public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(mathobj::Constant* backend WRITE setBackend MEMBER backend)
  Q_PROPERTY(bool playing WRITE setPlaying MEMBER playing NOTIFY playingChanged)
  Q_PROPERTY(LoopType loopType WRITE setLoopType MEMBER loopType)

  Q_PROPERTY(double from WRITE setFrom MEMBER from NOTIFY fromChanged)
  Q_PROPERTY(double to WRITE setTo MEMBER to NOTIFY toChanged)
  Q_PROPERTY(size_t steps WRITE setSteps MEMBER steps NOTIFY stepsChanged)
  Q_PROPERTY(double sliderPos WRITE setSliderPos MEMBER sliderPos NOTIFY sliderPosChanged)

  Q_PROPERTY(double value WRITE setValue READ getValue NOTIFY valueChanged)
  Q_PROPERTY(bool deadAndAlive READ isDeadAndAlive WRITE setDeadAndAlive NOTIFY deadAndAliveChanged)

public:

  enum LoopType {
    ONESHOT,
    REPEAT,
    PING_PONG
  };

  Q_ENUM(LoopType);

  explicit AnimatedConstant(QObject *parent = nullptr): QObject(parent) {};

  bool isPlaying() const {return playing;}
  bool isDeadAndAlive() const;

  double getFrom() const { return from; }
  double getTo() const { return to; }
  size_t getSteps() const { return steps; }

  Q_INVOKABLE double getValue() const { return backend->get_value(); }

  zg::mathobj::Constant* getBackend() { return backend; }

  bool isDiscreteRangeValid() const;

public slots:
  /// @brief make step, knowing that 'T' time has passed since the last step
  void animationStep(seconds T);

  void setValue(double);
  void setFrom(double);
  void setTo(double);
  void setSteps(size_t);
  void setSliderPos(double);
  void setDuration(double);
  void setPlaying(bool);
  void setLoopType(LoopType);
  void setDeadAndAlive(bool);

  void setBackend(mathobj::Constant*);

signals:
  void valueChanged();
  void fromChanged();
  void toChanged();
  void stepsChanged();
  void sliderPosChanged();
  void playingChanged();
  void deadAndAliveChanged();

protected:

  /// @brief ensure value is between from and to
  void constrainValue();
  void recomputeSliderPos();

  mathobj::Constant* backend = nullptr;
  LoopType loopType = ONESHOT;
  bool playing = false;
  bool deadAndAlive = false;
  double sliderPos = 0.5;
  seconds duration = seconds(5.);

  double from = std::nan("");
  double to = std::nan("");
  size_t steps = 5.;

  /// @brief it's sliderPos but in [0, 2] instead of [0, 1] interval
  ///        so it accounts for the ping-pong dynamic
  double phase = 0.5;

  static constexpr double lowerMul = 0.5;
  static constexpr double upperMul = 1.5;

};

}
