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

#include "BuildingBlocks/stateful.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {

using seconds = std::chrono::duration<double>;

class AnimatedConstant;

namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Constant: Stateful, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name)
  Q_PROPERTY(double from WRITE setFrom MEMBER from NOTIFY fromChanged)
  Q_PROPERTY(double value WRITE set_value MEMBER value NOTIFY valueChanged)
  Q_PROPERTY(double to WRITE setTo MEMBER to NOTIFY toChanged)
  Q_PROPERTY(int steps WRITE setSteps MEMBER steps NOTIFY stepsChanged)
  Q_PROPERTY(LoopType loopType WRITE setLoopType MEMBER loopType NOTIFY loopTypeChanged)
  Q_PROPERTY(bool deadAndAlive WRITE setDeadAndAlive MEMBER deadAndAlive NOTIFY deadAndAliveChanged)
  Q_PROPERTY(double duration WRITE setDuration READ getDuration NOTIFY durationChanged)

public:

  enum LoopType {
    ONESHOT,
    REPEAT,
    PING_PONG
  };

  Q_ENUM(LoopType);

  explicit Constant(QObject *parent = nullptr);
  ~Constant();

  Q_INVOKABLE State setName(QString name);
  QString getName() const { return input_name; }
  State sync();

  Q_INVOKABLE void set_value(double val);

  ///@brief sets the value as lerp between 'from' and 'to', with a in [0, 1]
  void setLerpValue(double a);

  void setFrom(double);
  void setTo(double);
  void setSteps(int);
  void setDuration(double);
  void setLoopType(LoopType);
  void setDeadAndAlive(bool);

  double getFrom() const { return from; }
  double getTo() const { return to; }
  int getSteps() const { return steps; }
  double getDuration() const { return duration.count(); }
  bool isDeadAndAlive() const { return deadAndAlive; }

  Q_INVOKABLE bool isValid();

  double get_value() const { return value; }

signals:
  void updated();
  void fromChanged();
  void valueChanged();
  void toChanged();
  void stepsChanged();
  void deadAndAliveChanged();
  void durationChanged();
  void loopTypeChanged();

protected:
  void constrainValue();

  double from = std::nan("");
  double value = std::nan("");
  double to = std::nan("");
  int steps = 5;

  LoopType loopType = ONESHOT;
  seconds duration = seconds(2.);
  bool deadAndAlive = false;

  QString input_name;

  static constexpr double lowerMul = 0.5;
  static constexpr double upperMul = 1.5;

  /// @brief so value can be changed without triggering signals
  ///        used in animations
  friend class zg::AnimatedConstant;
};

}
}
