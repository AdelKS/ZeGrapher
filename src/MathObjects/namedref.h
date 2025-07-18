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

#include "Utils/state.h"
#include "structures.h"

#include <optional>

#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {

/// @brief ZeGrapher proxy object that refers to another
struct NamedRef: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name)
  Q_PROPERTY(State state READ getState)

public:

  explicit NamedRef(QObject *parent = nullptr);

  void setSlot(size_t slot);
  State setName(QString name);
  QString getName() const { return input_name; }

  Q_INVOKABLE State getState() const;

  Q_INVOKABLE bool isValid() const;

  const zc::DynMathObject<zc_t>* getZcObject() const;

protected:
  QString input_name;
  std::optional<size_t> slot;
};

} // namespace mathobj
} // namespace zg
