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

#include "MathObjects/mathobject.h"

namespace zg {

MathObject::MathObject(QObject *parent, Type type)
  : QObject(parent), backend(createBackend(type))
{
  setType(type);
  connect(this, &MathObject::coordinateSystemChanged, this, &MathObject::updated);
}

void MathObject::setCoordinateSystem(CoordinateSystem s)
{
  std::visit(zc::utils::overloaded{
      [](mathobj::Constant*) {},
      [s](auto* v) { v->setCoordinateSystem(s); }
    },
    backend
  );
}


CoordinateSystem MathObject::getCoordinateSystem() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const mathobj::Constant*) { return CoordinateSystem::Cartesian; },
      [](const auto* v) { return v->getCoordinateSystem(); }
    },
    backend
  );
}

Base* MathObject::getBase()
{
  return std::visit(zc::utils::overloaded{
    [](Base* b) -> Base* { return b; },
    [](mathobj::Constant*) -> Base* { return nullptr; },
  }, backend);
}

PlotStyle* MathObject::getStyle()
{
  return const_cast<PlotStyle*>(std::as_const(*this).getStyle());
}

const PlotStyle* MathObject::getStyle() const
{
  return std::visit(zc::utils::overloaded{
    [](auto* b) -> PlotStyle* { return &b->style; },
    [](mathobj::Constant*) -> PlotStyle* { return nullptr; },
  }, backend);
}


MathObject::variant_t MathObject::createBackend(Type t)
{
  variant_t newBackend = [&]() -> variant_t {
    switch (t) {
      case EQUATION:
        return new mathobj::Equation(this);
      case CONSTANT:
        return new mathobj::Constant(this);
      case DATA:
        return new mathobj::Data(this);
      case PARAMETRIC:
        return new mathobj::Parametric(this);
      default: std::unreachable();
    }
  }();

  std::visit(
    zc::utils::overloaded{
      [this]<typename T>(T* n) {
        connect(n, &T::updated, this, &MathObject::updated);
        connect(n, &T::destroyed, this, &MathObject::updated);
        if constexpr (std::is_same_v<T, mathobj::Constant>)
        {

        }
        else
        {
          connect(n, &Base::schrodingerChanged, this, &MathObject::schrodingerChanged);
          connect(n, &Base::coordinateSystemChanged, this, &MathObject::coordinateSystemChanged);
          connect(n, &Base::discreteChanged, this, &MathObject::discreteChanged);
        }
      },
    },
    newBackend
  );

  return newBackend;
}

void MathObject::setType(Type t)
{
  const Type oldType = getType();

  if (oldType == t)
    return; // backend is already correct

  // delay delete the old pointer
  // so QML can properly stop using it
  std::visit(zc::utils::overloaded{
    [](auto* p){ p->deleteLater(); },
  }, backend);

  backend = createBackend(t);

  emit typeChanged();
  emit baseChanged();
  emit styleChanged();
}

MathObject::Type MathObject::getType() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const mathobj::Equation*) {
        return EQUATION;
      },
      [](const mathobj::Constant*) {
        return CONSTANT;
      },
      [](const mathobj::Data*) {
        return DATA;
      },
      [](const mathobj::Parametric*) {
        return PARAMETRIC;
      },
    },
    backend
  );
}

mathobj::Equation* MathObject::getEquation()
{
  return getBackend<mathobj::Equation>();
}

mathobj::Constant* MathObject::getConstant()
{
  return getBackend<mathobj::Constant>();
}

mathobj::Data* MathObject::getData()
{
  return getBackend<mathobj::Data>();
}


mathobj::Parametric* MathObject::getParametric()
{
  return getBackend<mathobj::Parametric>();
}

MathObject::EvalHandle MathObject::getZcObject() const
{
  using Ret = MathObject::EvalHandle;
  return std::visit(
    zc::utils::overloaded{
      [&](const auto* c) -> Ret {
        return &c->zcMathObj;
      },
      [](const mathobj::Parametric* p) -> Ret {
        return std::make_pair(p->obj1->getZcObject(), p->obj2->getZcObject());
      }
    },
    backend
  );

}

size_t MathObject::getRevision() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* e) {
        return e->zcMathObj.get_revision();
      },
      [](const mathobj::Parametric* p) {
        size_t rev1 = 0, rev2 = 0;
        if (const auto* o1 = p->obj1->getZcObject())
          rev1 = o1->get_revision();
        if (const auto* o2 = p->obj2->getZcObject())
          rev2 = o2->get_revision();
        return rev1 + rev2;
      }
    },
    backend
  );
}

bool MathObject::isValid() const
{
  return std::visit(zc::utils::overloaded{
    [](auto* e) { return e->isValid(); },
  }, backend);
}

QString MathObject::getName() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* e) {
        return e->getName();
      },
      [](const mathobj::Parametric*) {
        return QString();
      },
    },
    backend
  );
}

void MathObject::sync()
{
  std::visit(zc::utils::overloaded{
    [](std::monostate) {},
    [](auto* e) { e->sync(); },
  }, backend);
}

bool MathObject::isDiscrete() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const mathobj::Constant* c) { return c->isDiscrete(); },
      [](const Base* v) { return v->isDiscrete(); }
    },
    backend
  );
}

std::optional<MathObject::SamplingSettings> MathObject::getSamplingSettings()
{
  SamplingSettings settings;

  auto* base = getBase();
  if (not base) [[unlikely]]
    return {};

  settings.coordinateSystem = getCoordinateSystem();
  settings.range = base->getSnapshot();

  if (std::isnan(settings.range.min.v) or std::isnan(settings.range.max.v)) [[unlikely]]
    return {};

  if (settings.range.max <= settings.range.min)
    return {};

  settings.revision = getRevision();

  return settings;
}

void MathObject::setSchrodinger(bool s)
{
  std::visit(
    zc::utils::overloaded{
      [s](mathobj::Constant* c) { c->setDeadAndAlive(s); },
      [s](auto* v) { v->setSchrodinger(s); }
    },
    backend
  );
}

bool MathObject::isSchrodinger() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const mathobj::Constant* c) { return c->isDeadAndAlive(); },
      [](const auto* v) { return v->isSchrodinger(); }
    },
    backend
  );
}

MathObject::POD MathObject::exportPod() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const auto* c) -> POD { return c->exportPod(); },
    },
    backend
  );
}

void MathObject::importPod(POD p)
{
  std::visit(
    zc::utils::overloaded{
      [this](mathobj::Constant::POD&& p) { setType(CONSTANT); getConstant()->importPod(std::move(p)); },
      [this](mathobj::Data::POD&& p) { setType(DATA); getData()->importPod(std::move(p)); },
      [this](mathobj::Equation::POD&& p) { setType(EQUATION); getEquation()->importPod(std::move(p)); },
      [this](mathobj::Parametric::POD&& p) { setType(PARAMETRIC); getParametric()->importPod(std::move(p)); },
    },
    std::move(p)
  );
}

}
