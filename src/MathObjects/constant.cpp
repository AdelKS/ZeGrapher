/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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
}

void Constant::set_value(double val)
{
  if (value == val)
    return;

  zcMathObj = val;
  value = val;

  std::tie(from, to) = defaultBounds(val);

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

std::pair<double, double> Constant::defaultBounds(double value)
{
  // multiples of zero would leave the slider without a range
  if (value == 0)
    return {-zeroBound, zeroBound};

  return {lowerMul * value, upperMul * value};
}

Constant::POD Constant::exportPod() const
{
  const auto [defaultFrom, defaultTo] = defaultBounds(value);
  auto get_opt_edge = [](double edge, double defaultEdge) -> std::optional<double> {
    if (std::isnan(edge) or edge == defaultEdge)
      return {};
    return edge;
  };

  return POD {
    .name = yml::not_default(getName()),
    .value = yml::not_nan(value),
    .from = get_opt_edge(from, defaultFrom),
    .to = get_opt_edge(to, defaultTo),
    .steps = yml::not_default(steps, defaultSteps),
    .loop_type = yml::not_default(loopType, defaultLoopType),
    .schrodinger = yml::not_default(isDeadAndAlive(), false),
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
    setSteps(yml::clamped(*p.steps, minSteps, maxSteps));

  if (p.loop_type)
    setLoopType(*p.loop_type);

  if (p.schrodinger)
    setDeadAndAlive(*p.schrodinger);

  if (p.duration)
    setDuration(yml::clamped(*p.duration, minDuration, maxDuration));
}

}
}
