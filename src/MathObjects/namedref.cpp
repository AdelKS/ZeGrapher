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

#include "namedref.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "information.h"

namespace zg {
namespace mathobj {

NamedRef::NamedRef(QObject *parent): QObject(parent)
{
}

State NamedRef::setName(QString new_input_name)
{
  if (new_input_name == input_name)
    return sync();

  input_name = new_input_name;

  sync();

  emit updated();
  emit nameChanged();

  return getState();
}

void NamedRef::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

State NamedRef::sync()
{
  State newState;
  if (not input_name.isEmpty())
  {
    const auto* zcObj = getZcObject();
    if (zcObj)
    {
      if (*zcObj)
        newState.setValid();
      else newState.setInvalid(QObject::tr("Object \"%1\" is in an invalid state").arg(input_name.toHtmlEscaped()));
    }
    else newState.setInvalid(QObject::tr("Object not defined"));
  }

  setState(newState);

  return getState();
};

bool NamedRef::isDiscrete() const
{
  const auto* zcMathObj = getZcObject();
  return zcMathObj and *zcMathObj and shared::holdsDiscrete(*zcMathObj);
}

const zc::DynMathObject<zc_t>* NamedRef::getZcObject() const
{
  return zg::mathWorld->backend.get(input_name.toStdString());
}

} // namespace mathobj
} // namespace zg
