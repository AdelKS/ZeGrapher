#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include <QObject>
#include <QQmlEngine>

#include <zecalculator/error.h>

namespace zg {

/// @brief QML-exposed wrapper to std::optional<zc::Error>
struct OptError {
  Q_GADGET
  QML_ELEMENT

  Q_PROPERTY(Type type READ getType)
  Q_PROPERTY(QString message READ getMessage)

public:

  enum Type {NEUTRAL, VALID, INVALID};
  Q_ENUM(Type)

  Q_INVOKABLE Type getType() const;
  Q_INVOKABLE QString getMessage() const;

  bool operator == (const OptError&) const = default;

  std::optional<zc::Error> value = {};
};

}
