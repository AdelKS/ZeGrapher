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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "parametric.h"
#include "Utils/yaml.h"

namespace zg {
namespace mathobj {

Parametric::Parametric(QObject *parent)
  : zg::Base([](CoordinateSystem, bool){ return StringRange{"0", "10"}; }, parent),
    obj1(this),
    obj2(this)
{
  connect(&obj1, &NamedRef::updated, this, &Parametric::updated);
  connect(&obj2, &NamedRef::updated, this, &Parametric::updated);
}

void Parametric::sync()
{
  obj1.sync();
  obj2.sync();

  if (isValid())
    Base::setDiscrete(obj1.isDiscrete() or obj2.isDiscrete());
}

bool Parametric::isValid() const
{
  return obj1.isValid() and obj2.isValid();
}

EvalHandle Parametric::getZcObject() const
{
  return std::pair(obj1.getZcObject(), obj2.getZcObject());
}

Revision Parametric::getRevision() const
{
  auto of = [](const zc::DynMathObject<zc_t>* o) {
    return std::pair(o, o ? o->get_revision() : 0);
  };

  return {of(obj1.getZcObject()), of(obj2.getZcObject())};
}

Parametric::POD Parametric::exportPod() const
{
  POD p;
  p.first = yml::not_default(getFirst());
  p.second = yml::not_default(getSecond());
  exportCurve(p);
  return p;
}

void Parametric::importPod(Parametric::POD p)
{
  if (p.first)
    setFirst(QString::fromStdString(*p.first));

  if (p.second)
    setSecond(QString::fromStdString(*p.second));

  importCurve(p);
}

} // namespace mathobj
} // namespace zg
