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
#include "Utils/unit.h"
#include "constant.h"
#include "datasheet.h"
#include "equation.h"
#include "parametric.h"

namespace zg {

class MathWorld;

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(Type type WRITE setType READ getType NOTIFY typeChanged)
  Q_PROPERTY(bool schrodinger READ isSchrodinger NOTIFY schrodingerChanged)
  Q_PROPERTY(zg::Base* base READ getBase NOTIFY baseChanged)
  Q_PROPERTY(zg::PlotStyle* style READ getStyle NOTIFY styleChanged)

  /// @brief the backend of each type, null when the object has another type
  Q_PROPERTY(zg::mathobj::Equation* equation READ getEquation NOTIFY typeChanged)
  Q_PROPERTY(zg::mathobj::Constant* constant READ getConstant NOTIFY typeChanged)
  Q_PROPERTY(zg::mathobj::Parametric* parametric READ getParametric NOTIFY typeChanged)
  Q_PROPERTY(zg::mathobj::DataSheet* dataSheet READ getDataSheet NOTIFY typeChanged)

public:

  enum Type
  {
    EQUATION,
    CONSTANT,
    PARAMETRIC,
    DATASHEET
  };

  Q_ENUM(Type)

  explicit MathObject(QObject *parent = nullptr, Type type = EQUATION);

  void setType(Type);
  Type getType() const;

  zg::mathobj::Equation* getEquation();
  zg::mathobj::Constant* getConstant();
  zg::mathobj::Parametric* getParametric();
  zg::mathobj::DataSheet* getDataSheet();

  bool isSchrodinger() const;

  /// @brief the backend that draws a curve, nullptr for a constant or a sheet
  Base* getBase() const;

  PlotStyle* getStyle() const;

  /// @brief returns the asked for backend if it's the current backend, nullptr otherwise
  template <class T>
  T* getBackend();

  template <class T>
  const T* getBackend() const;

  using POD = std::variant<mathobj::Equation::POD,
                           mathobj::Constant::POD,
                           mathobj::Parametric::POD,
                           mathobj::DataSheet::POD>;

  POD exportPod() const;
  void importPod(POD);

public slots:
  /// @brief forwards the refresh() call to the current active backend
  void sync();

signals:
  void updated();
  void typeChanged();
  void schrodingerChanged();
  void baseChanged();
  void styleChanged();

protected:
  using variant_t = std::variant<mathobj::Equation*,
                                 mathobj::Constant*,
                                 mathobj::Parametric*,
                                 mathobj::DataSheet*>;

  /// @brief true when 'Backend' sits at index 't' of variant_t and POD
  template <Type t, class Backend>
  static constexpr bool atIndex
    = std::is_same_v<std::variant_alternative_t<t, variant_t>, Backend*>
      and std::is_same_v<std::variant_alternative_t<t, POD>, typename Backend::POD>;

  // getType() and MathWorld::importPod() read a Type from a variant index
  static_assert(atIndex<EQUATION, mathobj::Equation> and atIndex<CONSTANT, mathobj::Constant>
                and atIndex<PARAMETRIC, mathobj::Parametric> and atIndex<DATASHEET, mathobj::DataSheet>);

  variant_t createBackend(Type);

  variant_t backend;

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

template <>
struct glz::meta<zg::MathObject::POD> {
   static constexpr std::string_view tag = "type";
   static constexpr auto ids = std::array {"equation", "constant", "parametric", "datasheet"};
};
