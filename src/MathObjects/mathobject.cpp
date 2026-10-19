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
}

Base* MathObject::getBase() const
{
  return std::visit(zc::utils::overloaded{
    [](Base* b) -> Base* { return b; },
    [](mathobj::Constant*) -> Base* { return nullptr; },
    [](mathobj::DataSheet*) -> Base* { return nullptr; },
  }, backend);
}

PlotStyle* MathObject::getStyle() const
{
  Base* base = getBase();
  return base ? &base->style : nullptr;
}


MathObject::variant_t MathObject::createBackend(Type t)
{
  variant_t newBackend = [&]() -> variant_t {
    switch (t) {
      case EQUATION:
        return new mathobj::Equation(this);
      case CONSTANT:
        return new mathobj::Constant(this);
      case PARAMETRIC:
        return new mathobj::Parametric(this);
      case DATASHEET:
        return new mathobj::DataSheet(this);
      default: std::unreachable();
    }
  }();

  std::visit(
    zc::utils::overloaded{
      [this]<typename T>(T* n) {
        connect(n, &T::updated, this, &MathObject::updated);
        connect(n, &T::destroyed, this, &MathObject::updated);
        if constexpr (std::derived_from<T, Base>)
        {
          connect(n, &Base::schrodingerChanged, this, &MathObject::schrodingerChanged);
        }
        else if constexpr (std::same_as<T, mathobj::Constant>)
          connect(n, &mathobj::Constant::deadAndAliveChanged, this, &MathObject::schrodingerChanged);
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
      [](const mathobj::Parametric*) {
        return PARAMETRIC;
      },
      [](const mathobj::DataSheet*) {
        return DATASHEET;
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

mathobj::Parametric* MathObject::getParametric()
{
  return getBackend<mathobj::Parametric>();
}

mathobj::DataSheet* MathObject::getDataSheet()
{
  return getBackend<mathobj::DataSheet>();
}

void MathObject::sync()
{
  std::visit(zc::utils::overloaded{
    [](auto* e) { e->sync(); },
  }, backend);
}

bool MathObject::isSchrodinger() const
{
  return std::visit(
    zc::utils::overloaded{
      [](const mathobj::Constant* c) { return c->isDeadAndAlive(); },
      [](const mathobj::DataSheet*) { return false; },
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
      [this](mathobj::Equation::POD&& p) { setType(EQUATION); getEquation()->importPod(std::move(p)); },
      [this](mathobj::Parametric::POD&& p) { setType(PARAMETRIC); getParametric()->importPod(std::move(p)); },
      [this](mathobj::DataSheet::POD&& p) { setType(DATASHEET); getDataSheet()->importPod(std::move(p)); },
    },
    std::move(p)
  );
}

}
