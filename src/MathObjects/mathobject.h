#pragma once

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

#include <QtQmlIntegration/qqmlintegration.h>
#include <QObject>
#include <QSyntaxHighlighter>

#include "BuildingBlocks/base.h"
#include "Utils/plotstyle.h"
#include "constant.h"
#include "data.h"
#include "equation.h"
#include "parametric.h"

namespace zg {

class MathWorld;

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
  Q_PROPERTY(Type type WRITE setType READ getType NOTIFY typeChanged)
  Q_PROPERTY(bool discrete READ isDiscrete NOTIFY discreteChanged)
  Q_PROPERTY(bool schrodinger READ isSchrodinger NOTIFY schrodingerChanged)
  Q_PROPERTY(CoordinateSystem coordinateSystem WRITE setCoordinateSystem READ getCoordinateSystem NOTIFY coordinateSystemChanged)
  Q_PROPERTY(zg::Base* base READ getBase NOTIFY baseChanged)
  Q_PROPERTY(PlotStyle* style READ getStyle)

public:
  using EvalHandle
    = std::variant<std::monostate,
                   const zc::DynMathObject<zc_t>*,
                   std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*>>;

  enum Type
  {
    MONOSTATE,
    EQUATION,
    CONSTANT,
    DATA,
    PARAMETRIC
  };

  Q_ENUM(Type);

  struct SamplingSettings {
    zg::real_range1d range = {.min = {0.}, .max = {0.}};
    zg::CoordinateSystem coordinateSystem = zg::CoordinateSystem::Cartesian;
    size_t revision = 0;

    bool operator == (const SamplingSettings&) const = default;
  };

  explicit MathObject(QObject *parent = nullptr, Type type = EQUATION);

  Q_INVOKABLE void setType(Type);
  Q_INVOKABLE Type getType() const;

  bool isValid() const { return state.isValid(); };
  QString getName() const { return name; };

  size_t getRevision() const;

  EvalHandle getZcObject() const;

  void setSchrodinger(bool);

  Q_INVOKABLE mathobj::Equation* getEquation();
  Q_INVOKABLE mathobj::Constant* getConstant();
  Q_INVOKABLE mathobj::Data* getData();
  Q_INVOKABLE mathobj::Parametric* getParametric();

  bool isContinuous() const { return not isDiscrete(); };

  Q_INVOKABLE bool isDiscrete() const;
  Q_INVOKABLE bool isSchrodinger() const { return schrodinger; }

  Q_INVOKABLE void setCoordinateSystem(CoordinateSystem);
  CoordinateSystem getCoordinateSystem() const;

  Base* getBase();

  PlotStyle* getStyle() { return &style; }

  std::optional<SamplingSettings> getSamplingSettings();

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
  T* getBackend();

  template <class T>
  const T* getBackend() const;

  PlotStyle style;

public slots:
  /// @brief forwards the refresh() call to the current active backend
  State sync();

signals:
  void stateChanged();
  void updated();
  void typeChanged();
  void nameChanged();
  void discreteChanged();
  void schrodingerChanged();
  void coordinateSystemChanged();
  void baseChanged();

protected:
  std::variant<std::monostate,
               mathobj::Equation*,
               mathobj::Constant*,
               mathobj::Data*,
               mathobj::Parametric*> backend;
  QString name;
  State state;

  bool schrodinger = false;

  friend zg::MathWorld;
};

template <class T>
const T* MathObject::getBackend() const
{
  if (std::holds_alternative<T*>(backend))
    return std::get<T*>(backend);
  else return nullptr;
}

template <class T>
T* MathObject::getBackend()
{
  return const_cast<T*>(std::as_const(*this).getBackend<T>());
}

}
