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

#include "MathObjects/expr.h"
#include "Utils/plotstyle.h"
#include "Utils/unit.h"

#include <glaze/yaml.hpp>

#include <array>
#include <concepts>

namespace zg {

  using EvalHandle
    = std::variant<const zc::DynMathObject<zc_t>*,
                   std::pair<const zc::DynMathObject<zc_t>*, const zc::DynMathObject<zc_t>*>>;

  /// @brief the objects that a curve evaluates, each with its revision
  /// @note ZeCalculator counts revisions per object, so a curve that switches to
  ///       another object can keep the same numbers
  using Revision = std::array<std::pair<const zc::DynMathObject<zc_t>*, size_t>, 2>;

struct SamplingSettings;

struct StringRange {
  QString start, end;
  bool operator == (const StringRange&) const = default;
};

/// @brief the shared base of the backends that draw a curve: equations,
///        parametric equations and data columns
struct Base: QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("abstract base of math object backends, only exposed for enum and property access")

  Q_PROPERTY(mathobj::Expr* start READ getStartPtr CONSTANT)
  Q_PROPERTY(mathobj::Expr* end READ getEndPtr CONSTANT)
  Q_PROPERTY(bool discrete WRITE setDiscrete MEMBER discrete NOTIFY discreteChanged)
  Q_PROPERTY(CoordinateSystem coordinateSystem WRITE setCoordinateSystem MEMBER coordinateSystem NOTIFY coordinateSystemChanged)
  Q_PROPERTY(bool schrodinger WRITE setSchrodinger MEMBER schrodinger NOTIFY schrodingerChanged)
  Q_PROPERTY(PlotStyle* style READ getStylePtr CONSTANT)

public:
  enum CoordinateSystem {Cartesian, Polar};
  Q_ENUM(CoordinateSystem);

  /// @returns stringRange given a coordinate system and is_discrete boolean
  using DefaultStringRangeProvider = std::function<StringRange(CoordinateSystem, bool)>;
  explicit Base(DefaultStringRangeProvider, QObject* parent = nullptr);

  mathobj::Expr* getStartPtr() { return &start; }
  mathobj::Expr* getEndPtr() { return &end; }
  PlotStyle* getStylePtr() { return &style; }

  void setCoordinateSystem(CoordinateSystem);
  void setDiscrete(bool);
  void setSchrodinger(bool);

  void setStart(QString s) { start.setExpression(s); }
  void setEnd(QString s) { end.setExpression(s); }

  QString getStartStr() const { return start.getExpression(); }
  QString getEndStr() const { return end.getExpression(); }

  CoordinateSystem getCoordinateSystem() const { return coordinateSystem; }
  bool isDiscrete() const { return discrete; }
  bool isSchrodinger() const { return schrodinger; }

  zg::real_range1d getSnapshot() const;

  /// @brief getSnapshot, but checks its validity
  std::optional<zg::real_range1d> getValidSnapshot() const;

  /// @brief the object's sampling settings, nothing when its range is
  ///        invalid
  /// @param revision  the revision of what the object evaluates
  std::optional<SamplingSettings> getSamplingSettings(Revision revision) const;

  bool isDefaultStart() const;
  bool isDefaultEnd() const;

  void resetStart();
  void resetEnd();

  StringRange getDefaultStringRange() const;

  PlotStyle style;

signals:
  void coordinateSystemChanged();
  void discreteChanged();
  void schrodingerChanged();

protected:

  mathobj::Expr start;
  mathobj::Expr end;
  DefaultStringRangeProvider defaultStringRangeProvider;
  CoordinateSystem coordinateSystem = Cartesian;
  bool discrete = false;
  bool schrodinger = false;
};

using CoordinateSystem = Base::CoordinateSystem;

struct SamplingSettings {
  zg::real_range1d range = {.min = {0.}, .max = {0.}};
  CoordinateSystem coordinateSystem = CoordinateSystem::Cartesian;
  Revision revision = {};

  bool operator == (const SamplingSettings&) const = default;
};

/// @brief a backend that the sampler draws
template <class T>
concept PlottableBackend = std::derived_from<T, Base> and requires(const T& t) {
  { t.getZcObject() } -> std::same_as<EvalHandle>;
  { t.getRevision() } -> std::same_as<Revision>;
  { t.isValid() } -> std::same_as<bool>;
};

}

template <>
struct glz::meta<zg::Base::CoordinateSystem>
{
   using enum zg::Base::CoordinateSystem;
   static constexpr auto value = glz::enumerate(
      "cartesian",  Cartesian,
      "polar",   Polar);
};
